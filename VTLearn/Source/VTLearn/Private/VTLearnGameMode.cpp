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
	UE_LOG(LogTemp, Log, TEXT("AVTLearnGameMode - Loading level info"));

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


	// Load distractors
	TArray<UPhoneticPhrase*> AllPhrases;
	DistractorPhrases.Empty();
	for(FString PhraseName : GameInstance->CurrentLevelStatus->LevelConfig.DistractorPhrases)
	{
		TArray<UPhoneticPhrase*> Vtts = UPhoneticPhrase::LoadPhrases(PhraseName);
		for(UPhoneticPhrase* Vtt : Vtts)
		{
			DistractorPhrases.Add(PhraseName, Vtt);
		}
	}

	// Load training phrases
	TrainingPhrases.Empty();
	for(FString PhraseName : GameInstance->CurrentLevelStatus->LevelConfig.TrainingPhrases)
	{
		TArray<UPhoneticPhrase*> Vtts = UPhoneticPhrase::LoadPhrases(PhraseName);
		for(UPhoneticPhrase* Vtt : Vtts)
		{
			TrainingPhrases.Add(PhraseName, Vtt);
		}
	}
	bLoaded = true;
	LevelDataLoaded.Broadcast();
}

void AVTLearnGameMode::SetupGenerators()
{
	TArray<UPhoneticPhrase*> TrainingValuesArray;
	TrainingPhrases.GenerateValueArray(TrainingValuesArray);

	TArray<UPhoneticPhrase*> DistractorValuesArray;
	DistractorPhrases.GenerateValueArray(DistractorValuesArray);

	for (TActorIterator<AVibeyItemGenerator> GeneratorItr(GetWorld()); GeneratorItr; ++GeneratorItr)
	{
		AVibeyItemGenerator* ItemGenerator = *GeneratorItr;

		ItemGenerator->AddToPhraseBank(TrainingValuesArray);
		ItemGenerator->AddToPhraseBank(DistractorValuesArray);

		UE_LOG(LogTemp, Log, TEXT("Set up generator %s"), *ItemGenerator->GetName());
	}
}

void AVTLearnGameMode::SetupReceivers()
{
	UE_LOG(LogTemp, Log, TEXT("Setting up receivers"));
	TActorIterator<AVibeyItemReceiver> ReceiverItr(GetWorld());

	TArray<FString> PhraseStrings;
	TrainingPhrases.GetKeys(PhraseStrings);

	UE_LOG(LogTemp, Log, TEXT("There are %d PhraseStrings"), PhraseStrings.Num());

	for(FString PhraseText : PhraseStrings)
	{
		UE_LOG(LogTemp, Log, TEXT("Searching for Vtts for '%s'"), *PhraseText);

		// find vtts for this phrase
		TArray<UPhoneticPhrase*> Vtts;
		TrainingPhrases.MultiFind(PhraseText, Vtts);
		if(Vtts.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Could not find any VTTs for '%s'"), ANSI_TO_TCHAR(__FUNCTION__), *PhraseText);
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("Searching for available receiver for '%s'"), *PhraseText);
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
		UE_LOG(LogTemp, Log, TEXT("VTTs and receiver found for '%s'"), *PhraseText);
		ReceiverItr->SetMatchPhrases(Vtts);
		++ReceiverItr;
		UE_LOG(LogTemp, Log, TEXT("Done with '%s'"), *PhraseText);
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