// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "GameFramework/Actor.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VTPlayerController.generated.h"

/**
 *
 */
UCLASS()
class VTLEARN_API AVTPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintReadOnly)
	AActor* HeldItem;

	virtual void SetupInputComponent() override;

	void OnMoveUp(float Value);
	void OnMoveRight(float Value);
	void OnJump();
	void OnGrab();
	void OnInteract();

	UFUNCTION(BlueprintCallable)
	bool CanHold(AActor* Item);

	UFUNCTION(BlueprintCallable)
	bool HoldItem(AActor* Item);

	UFUNCTION(BlueprintCallable)
	void DropItem();
};
