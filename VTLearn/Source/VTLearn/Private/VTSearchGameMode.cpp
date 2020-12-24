// Fill out your copyright notice in the Description page of Project Settings.


#include "VTSearchGameMode.h"

#include "VTLearnGameMode.h"
#include "VibeyItemReceiver.h"
#include "VTHintDisplayer.h"
#include "VTPlayerSearchController.h"

#include "Engine/StaticMeshActor.h"
#include "EngineUtils.h"
#include "Math/UnrealMathUtility.h"

AVTSearchGameMode::AVTSearchGameMode() : AVTLearnGameMode()
{
	PlayerControllerClass = AVTPlayerSearchController::StaticClass();
}


void AVTSearchGameMode::BeginPlay()
{
	Super::BeginPlay();

	Matcher = NewObject<UPhoneSequenceMatcher>();

	TArray<FString> TmpPhraseKeys;
	TrainingPhrases.GenerateKeyArray(TmpPhraseKeys); // @TODO: shuffle?
	for(FString PhraseText : TmpPhraseKeys)
	{
		PhraseKeys.AddUnique(PhraseText);
	}
	// Shuffle
	for (int32 i = 0; i < PhraseKeys.Num(); i++)
	{
		int32 j = FMath::RandRange(i, PhraseKeys.Num()-1);
		if (i != j)
		{
			PhraseKeys.Swap(i, j);
		}
	}

	float InitialDelay = 2.0f;
	float TimePerPhrase = (RemainingTime - InitialDelay) / PhraseKeys.Num();

	UE_LOG(LogTemp, Log, TEXT("AVTSearchGameMode::BeginPlay - %d phrases @ %0.2fs"), PhraseKeys.Num(), TimePerPhrase);

	GetWorld()->GetTimerManager().SetTimer(NextPhraseTimerHandle, this, &AVTSearchGameMode::NextPhrase, TimePerPhrase, true, InitialDelay);
}

void AVTSearchGameMode::NextPhrase()
{
	CurrentPhraseIdx++;
	ItemPool.Empty();

	if(CurrentPhraseIdx >= PhraseKeys.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("No more training phrases left!"));
		return;
	}

	FString PhraseText = PhraseKeys[CurrentPhraseIdx];
	UE_LOG(LogTemp, Log, TEXT("Starting next phrase: %s"), *PhraseText);

	TArray<UPhoneticPhrase*> Phrases;
	TrainingPhrases.MultiFind(PhraseText, Phrases);
	Matcher->SetMatchPhrases(Phrases);

	for (TActorIterator<AActor> HinterItr(GetWorld()); HinterItr; ++HinterItr)
	{
		if(IVTHintDisplayer* Hinter = Cast<IVTHintDisplayer>(*HinterItr))
		{
			UE_LOG(LogTemp, Log, TEXT("I found a hint giver!"));
			Hinter->Execute_ShowHint(*HinterItr, PhraseText);
		}
	}
}

