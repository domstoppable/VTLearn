// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PhoneticPhrase.h"
#include "VTPlayerState.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VTLearnGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVTLevelTimedOut);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVTLevelDataLoadCompleted);

UCLASS()
class VTLEARN_API AVTLearnGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVTLearnGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float Delta) override;

	TMultiMap<FString, UPhoneticPhrase*> DistractorPhrases;
	TMultiMap<FString, UPhoneticPhrase*> TrainingPhrases;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString VTTPath = TEXT("/Game/Data/VTT");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bLoaded = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RemainingTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PointsForCorrect = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PointsForIncorrect = -5;

	UPROPERTY(BlueprintAssignable)
	FVTLevelTimedOut LevelTimedOut;

	UPROPERTY(BlueprintAssignable)
	FVTLevelDataLoadCompleted LevelDataLoaded;

	UFUNCTION(BlueprintCallable)
	void QuitLevel();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetHumanReadableRemainingTime();

	UFUNCTION(BlueprintPure, BlueprintCallable)
	const TArray<UPhoneticPhrase*> GetTrainingPhrases()
	{
		UE_LOG(LogTemp, Log, TEXT("Training phrases requested... there are %d of them"), TrainingPhrases.Num());
		TArray<UPhoneticPhrase*> Phrases;
		TrainingPhrases.GenerateValueArray(Phrases);

		return Phrases;
	}

protected:
	virtual void LoadLevelInfo();
	virtual void SetupGenerators();
	virtual void SetupReceivers();
};
