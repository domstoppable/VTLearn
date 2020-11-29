#include "VTLearnGameMode.h"

#include "VTGameInstance.h"
#include "VTPlayerController.h"
#include "VTLearnCharacter.h"
#include "VTPlayerState.h"
#include "VTHUD.h"
#include "VibeyItemReceiver.h"
#include "VibeyItemGenerator.h"
#include "PhoneticPhrase.h"

#include "EngineUtils.h"

AVTLearnGameMode::AVTLearnGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerStateClass = AVTPlayerState::StaticClass();
	PlayerControllerClass = AVTPlayerController::StaticClass();
	DefaultPawnClass = AVTLearnCharacter::StaticClass();
	HUDClass = AVTHUD::StaticClass();

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/VTT/VTPlayerControllerBP"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

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
	}
}

void AVTLearnGameMode::BeginPlay()
{
	Super::BeginPlay();

	UVTGameInstance* GameInstance = Cast<UVTGameInstance>(GetGameInstance());
	if(!IsValid(GameInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s - Unexpected GameInstance class"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("%s - Setting up level %s"), ANSI_TO_TCHAR(__FUNCTION__), *(GameInstance->CurrentLevelConfig.Name));
	UE_LOG(LogTemp, Log, TEXT("%s - %d to train, %d to distract"), ANSI_TO_TCHAR(__FUNCTION__), GameInstance->CurrentLevelConfig.TrainingPhrases.Num(), GameInstance->CurrentLevelConfig.DistractorPhrases.Num());

	RemainingTime = GameInstance->CurrentLevelConfig.TimeLimit;
	// Load distractors
	PhraseBank.Empty();
	for(FString PhraseName : GameInstance->CurrentLevelConfig.DistractorPhrases)
	{
		PhraseBank.Append(UPhoneticPhrase::LoadPhrases(PhraseName));
	}

	// load training phrases
	int32 PhraseIndex = -1;
	TActorIterator<AVibeyItemReceiver> ReceiverItr(GetWorld());
	for(FString PhraseName : GameInstance->CurrentLevelConfig.TrainingPhrases)
	{
		TArray<UPhoneticPhrase*> Vtts = UPhoneticPhrase::LoadPhrases(PhraseName);
		if(Vtts.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s Could not find any VTTs for %s"), ANSI_TO_TCHAR(__FUNCTION__), *PhraseName);
			continue;
		}
		while(ReceiverItr && !ReceiverItr->AllowAutoAssign)
		{
			++ReceiverItr;
		}

		if(!ReceiverItr)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s More phrases than receivers!"), ANSI_TO_TCHAR(__FUNCTION__));
			break;
		}

		ReceiverItr->SetMatchPhrase(Vtts[0]);
		TrainingPhrases.Append(Vtts);

		++ReceiverItr;
	}

	PhraseBank.Append(TrainingPhrases);

	UE_LOG(LogTemp, Log, TEXT("Starting game with %d training phrases, %d phrases total"), TrainingPhrases.Num(), PhraseBank.Num());

	for (TActorIterator<AVibeyItemGenerator> GeneratorItr(GetWorld()); GeneratorItr; ++GeneratorItr)
	{
		AVibeyItemGenerator* ItemGenerator = *GeneratorItr;
		ItemGenerator->Phrases = PhraseBank;
	}
}
