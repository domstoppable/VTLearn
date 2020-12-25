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


	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AVTLearnGameMode::TickAfterBeginPlay);
}

void AVTLearnGameMode::TickAfterBeginPlay()
{
	LoadLevelInfo();
	SetupGenerators();
	SetupReceivers();
}

void AVTLearnGameMode::LoadLevelInfo()
{

	UVTGameInstance* GameInstance = Cast<UVTGameInstance>(GetGameInstance());
	if(!IsValid(GameInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Unexpected GameInstance class"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	if(!IsValid(GameInstance->CurrentLevelStatus))
	{
		UE_LOG(LogTemp, Log, TEXT("%s : No level data loaded!"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("%s - Setting up level %s"), ANSI_TO_TCHAR(__FUNCTION__), *(GameInstance->CurrentLevelStatus->LevelConfig.Name));
	UE_LOG(LogTemp, Log, TEXT("%s - %d to train, %d to distract"), ANSI_TO_TCHAR(__FUNCTION__), GameInstance->CurrentLevelStatus->LevelConfig.TrainingPhrases.Num(), GameInstance->CurrentLevelStatus->LevelConfig.DistractorPhrases.Num());

	RemainingTime = GameInstance->CurrentLevelStatus->LevelConfig.TimeLimit;

	// Load distractors
	DistractorPhrases.Empty();
	for(FString PhraseName : GameInstance->CurrentLevelStatus->LevelConfig.DistractorPhrases)
	{
		for(UPhoneticPhrase* Phrase : UPhoneticPhrase::LoadPhrases(PhraseName))
		{
			DistractorPhrases.Add(PhraseName, Phrase);
		}

	}

	// Load training phrases
	TrainingPhrases.Empty();
	for(FString PhraseName : GameInstance->CurrentLevelStatus->LevelConfig.TrainingPhrases)
	{
		for(UPhoneticPhrase* Phrase : UPhoneticPhrase::LoadPhrases(PhraseName))
		{
			TrainingPhrases.Add(PhraseName, Phrase);
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

		UE_LOG(LogTemp, Log, TEXT("Set up generator %s"), *ItemGenerator->GetActorLabel());
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

