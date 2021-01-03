// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VTPlayerState.h"
#include "VTMenuController.h"

#include "GameFramework/Actor.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VTPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVTPauseChanged, bool, Paused);

UCLASS()
class VTLEARN_API AVTPlayerController : public AVTMenuController
{
	GENERATED_BODY()

public:
	AVTPlayerController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// @TODO: move this to character class
	UPROPERTY(BlueprintReadOnly)
	AActor* HeldItem;

	UPROPERTY(BlueprintAssignable)
	FVTPauseChanged PauseChanged;

	virtual void SetupInputComponent() override;

	void OnMoveUp(float Value);
	void OnMoveRight(float Value);
	void OnJump();
	void OnGrab();
	void OnRevibe();
	void OnInteract();

	UFUNCTION(BlueprintCallable)
	void TogglePause();

	virtual void Pause() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual FVector GetForward2D();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanInteract();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanRevibe();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanHold(AActor* Item);

	UFUNCTION(BlueprintCallable)
	virtual bool HoldItem(AActor* Item);

	UFUNCTION(BlueprintCallable)
	void DropItem();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AVTPlayerState* GetVTPlayerState();

	UFUNCTION()
	void OnLevelTimedOut();

	UFUNCTION(BlueprintPure, meta = (WorldContext = WorldContextObject))
	static AVTPlayerController* GetVTPlayerController(UObject* WorldContextObject);

};
