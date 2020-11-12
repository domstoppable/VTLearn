// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VTLearnCharacter.generated.h"

UCLASS()
class VTLEARN_API AVTLearnCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AVTLearnCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void ItemGrabbed(AActor* Item);

	UFUNCTION(BlueprintImplementableEvent)
	void ItemDropped(AActor* Item);
};
