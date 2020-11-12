// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PhonePhraseMatcher.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VibeyItemReceiver.generated.h"

UCLASS()
class VTLEARN_API AVibeyItemReceiver : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVibeyItemReceiver();

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(BlueprintReadWrite)
	FString HelpText;

	UPROPERTY(BlueprintReadWrite)
	UPhoneSequenceMatcher* Matcher;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
