#include "VTLearnGameMode.h"

#include "VTGameInstance.h"
#include "VTPlayerController.h"
#include "VTLearnCharacter.h"
#include "VTPlayerState.h"
#include "VTHUD.h"
#include "VibeyItemReceiver.h"
#include "VibeyItemGenerator.h"
#include "PhoneticPhrase.h"

#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "TimerManager.h"

AVTLearnGameMode::AVTLearnGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerStateClass = AVTPlayerState::StaticClass();
	PlayerControllerClass = AVTPlayerController::StaticClass();
	DefaultPawnClass = AVTLearnCharacter::StaticClass();
	HUDClass = AVTHUD::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/GameCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AHUD> HUDBPClass(TEXT("/Game/UI/VTHUDBP"));
	if (HUDBPClass.Class != NULL)
	{
		HUDClass = HUDBPClass.Class;
	}
}

void AVTLearnGameMode::Tick(float Delta)
{
	RemainingTime -= Delta;
	if(RemainingTime <= 0.0f)
	{
		LevelTimedOut.Broadcast();
		Cast<UVTGameInstance>(GetGameInstance())->SaveProgress();
	}
}

void AVTLearnGameMode::BeginPlay()
{
	Super::BeginPlay();

	LoadLevelInfo();
	SetupGenerators();
	SetupReceivers();
}

void AVTLearnGameMode::LoadLevelInfo()
{
	if(!CheckStillInWorld())
	{
		return;
	}

	UVTGameInstance* GameInstance = Cast<UVTGameInstance>(GetGameInstance());
	if(!IsValid(GameInstance->CurrentLevelStatus))
	{
		UE_LOG(LogTemp, Log, TEXT("%s : No level data loaded!"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("%s - Setting up level %s"), ANSI_TO_TCHAR(__FUNCTION__), *(GameInstance->CurrentLevelStatus->LevelConfig.Name));
	UE_LOG(LogTemp, Log, TEXT("%s - %d to train, %d to distract"), ANSI_TO_TCHAR(__FUNCTION__), GameInstance->CurrentLevelStatus->LevelConfig.TrainingPhrases.Num(), GameInstance->CurrentLevelStatus->LevelConfig.DistractorPhrases.Num());

	RemainingTime = GameInstance->CurrentLevelStatus->LevelConfig.TimeLimit;


	// Load phrases
	TArray<FString> Prefixes;
	Prefixes.Add(TEXT("f1"));
	Prefixes.Add(TEXT("f2"));
	Prefixes.Add(TEXT("m1"));
	Prefixes.Add(TEXT("m2"));
	// Load distractors
	TArray<UPhoneticPhrase*> AllPhrases;
	DistractorPhrases.Empty();
	for(FString PhraseName : GameInstance->CurrentLevelStatus->LevelConfig.DistractorPhrases)
	{
		for(FString Prefix : Prefixes)
		{
			FString ReferencePath = FString::Printf(TEXT("%s/%s-%s"), *VTTPath, *Prefix, *PhraseName);
			UPhoneticPhrase* Phrase = LoadObject<UPhoneticPhrase>(NULL, *ReferencePath, NULL, LOAD_None, NULL);
			if(IsValid(Phrase))
			{
				DistractorPhrases.Add(PhraseName, Phrase);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to find phrase `%s`"), *ReferencePath);
			}
		}
	}

	// Load training phrases
	TrainingPhrases.Empty();
	for(FString PhraseName : GameInstance->CurrentLevelStatus->LevelConfig.TrainingPhrases)
	{
		for(FString Prefix : Prefixes)
		{
			FString ReferencePath = FString::Printf(TEXT("%s/%s-%s"), *VTTPath, *Prefix, *PhraseName);
			UPhoneticPhrase* Phrase = LoadObject<UPhoneticPhrase>(NULL, *ReferencePath, NULL, LOAD_None, NULL);
			if(IsValid(Phrase))
			{
				TrainingPhrases.Add(PhraseName, Phrase);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to find phrase `%s`"), *ReferencePath);
			}
		}
	}
}

void AVTLearnGameMode::SetupGenerators()
{
	for (TActorIterator<AVibeyItemGenerator> GeneratorItr(GetWorld()); GeneratorItr; ++GeneratorItr)
	{
		AVibeyItemGenerator* ItemGenerator = *GeneratorItr;

		TArray<UPhoneticPhrase*> TmpArray;

		TrainingPhrases.GenerateValueArray(TmpArray);
		ItemGenerator->AddToPhraseBank(TmpArray);

		DistractorPhrases.GenerateValueArray(TmpArray);
		ItemGenerator->AddToPhraseBank(TmpArray);

		UE_LOG(LogTemp, Log, TEXT("Set up generator %s"), *ItemGenerator->GetName());
	}
}

void AVTLearnGameMode::SetupReceivers()
{
	TActorIterator<AVibeyItemReceiver> ReceiverItr(GetWorld());

	TArray<FString> PhraseStrings;
	TrainingPhrases.GetKeys(PhraseStrings);

	for(FString PhraseText : PhraseStrings)
	{
		// find vtts for this phrase
		TArray<UPhoneticPhrase*> Vtts;
		TrainingPhrases.MultiFind(PhraseText, Vtts);
		if(Vtts.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Could not find any VTTs for %s"), ANSI_TO_TCHAR(__FUNCTION__), *PhraseText);
			continue;
		}

		// find a receiver to receive them
		while(ReceiverItr && !ReceiverItr->AllowAutoAssign)
		{
			++ReceiverItr;
		}

		if(!ReceiverItr)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s More phrases than receivers!"), ANSI_TO_TCHAR(__FUNCTION__));
			break;
		}

		// setup that receiver with those phrases
		ReceiverItr->SetMatchPhrases(Vtts);
		++ReceiverItr;
	}

	// destroy unused receivers
	while(ReceiverItr)
	{
		if(ReceiverItr->AllowAutoAssign)
		{
			GetWorld()->DestroyActor(*ReceiverItr);
		}
		++ReceiverItr;
	}
}

FString AVTLearnGameMode::GetHumanReadableRemainingTime()
{
	int Minutes = RemainingTime / 60;
	int Seconds = RemainingTime - (Minutes*60);

	return FString::Printf(TEXT("%d:%02d"), Minutes, Seconds);
}

void AVTLearnGameMode::QuitLevel()
{
	RemainingTime = 0.01;
}