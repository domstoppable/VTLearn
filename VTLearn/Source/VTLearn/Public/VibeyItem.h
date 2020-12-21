// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"

#include "PhoneticPhrase.h"

#include "VibeyItem.generated.h"

UCLASS(BlueprintType)
class VTLEARN_API AVibeyItem : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AVibeyItem();

	UPROPERTY(BlueprintReadWrite)
	UPhoneticPhrase* Phrase;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;



};
