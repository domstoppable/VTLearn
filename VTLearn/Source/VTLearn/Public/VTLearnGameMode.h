// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PhoneticPhrase.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VTLearnGameMode.generated.h"

/**
 *
 */
UCLASS()
class VTLEARN_API AVTLearnGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AVTLearnGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float Delta) override;

	UPROPERTY(BlueprintReadWrite)
	TArray<UPhoneticPhrase*> PhraseBank;

	UPROPERTY(BlueprintReadWrite)
	TArray<UPhoneticPhrase*> TrainingPhrases;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RemainingTime = 180.0;
};
