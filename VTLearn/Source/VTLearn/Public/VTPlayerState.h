// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "VTPlayerState.generated.h"

UCLASS()
class VTLEARN_API AVTPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CorrectCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 IncorrectCount = 0;
};
