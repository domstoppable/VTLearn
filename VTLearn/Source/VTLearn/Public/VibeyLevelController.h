// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VibeyItemGenerator.h"
#include "VibeyItemReceiver.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VibeyLevelController.generated.h"

UCLASS()
class VTLEARN_API AVibeyLevelController : public AActor
{
	GENERATED_BODY()

public:
	AVibeyLevelController();

	UPROPERTY(EditAnywhere)
	FString VTTDirectory;

	UPROPERTY(EditAnywhere)
	TArray<AVibeyItemGenerator*> Generators;

	UPROPERTY(EditAnywhere)
	TArray<AVibeyItemReceiver*> Receivers;

	UPROPERTY(BlueprintReadWrite)
	TArray<UPhoneticPhrase*> AllPhrases;

	UPROPERTY(BlueprintReadWrite)
	TArray<UPhoneticPhrase*> SelectedPhrases;

	UFUNCTION()
	void OnDeviceConnected();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};
