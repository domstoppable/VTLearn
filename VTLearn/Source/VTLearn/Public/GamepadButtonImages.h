// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Engine/Texture2D.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GamepadButtonImages.generated.h"



UCLASS(BlueprintType)
class VTLEARN_API UGamepadButtonImages : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* FaceTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* FaceRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* FaceBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* FaceLeft;

	UFUNCTION(BlueprintPure)
	UTexture2D* FromKey(FKey Key);

	UFUNCTION(BlueprintPure)
	UTexture2D* FromAction(FName Action);
};
