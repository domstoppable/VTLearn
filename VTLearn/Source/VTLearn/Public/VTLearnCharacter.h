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

	void MoveUp(float Value);
	void MoveRight(float Value);

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
