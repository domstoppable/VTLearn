// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PhoneticPhrase.h"
#include "VTPlayerState.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VTLearnGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVTLevelTimedOut);

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
	float RemainingTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PointsForCorrect = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PointsForIncorrect = -5;

	UPROPERTY(BlueprintReadOnly)
	FVTLevelTimedOut LevelTimedOut;

	UFUNCTION(BlueprintCallable)
	void QuitLevel();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FString GetHumanReadableRemainingTime();

	UFUNCTION(BlueprintPure, BlueprintCallable)
	const TArray<UPhoneticPhrase*> GetTrainingPhrases()
	{
		TArray<UPhoneticPhrase*> Phrases;
		TrainingPhrases.GenerateValueArray(Phrases);

		return Phrases;
	}

protected:
	virtual void LoadLevelInfo();
	virtual void SetupGenerators();
	virtual void SetupReceivers();
};
