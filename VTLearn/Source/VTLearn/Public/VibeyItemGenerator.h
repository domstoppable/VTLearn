// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PhoneticPhrase.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VibeyItemGenerator.generated.h"

UCLASS()
class VTLEARN_API AVibeyItemGenerator : public AActor
{
	GENERATED_BODY()

public:
	AVibeyItemGenerator();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite)
	TArray<UPhoneticPhrase*> Phrases;

	UFUNCTION(BlueprintCallable)
	UPhoneticPhrase* RandomPhrase();

};
