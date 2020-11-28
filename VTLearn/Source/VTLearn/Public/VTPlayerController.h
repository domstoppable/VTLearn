// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VTPlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVTControllerScoreChanged, int32, Delta, int32, Total);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVTPauseChanged, bool, Paused);

UCLASS()
class VTLEARN_API AVTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	typedef APlayerController super;

	AVTPlayerController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintReadOnly)
	AActor* HeldItem;

	UPROPERTY(BlueprintReadWrite)
	int32 Score;

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
	bool CanInteract();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanRevibe();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanHold(AActor* Item);

	UFUNCTION(BlueprintCallable)
	bool HoldItem(AActor* Item);

	UFUNCTION(BlueprintCallable)
	void DropItem();

	UFUNCTION(BlueprintCallable)
	int32 AdjustScore(int32 Amount);

	UPROPERTY(BlueprintAssignable)
	FVTControllerScoreChanged ScoreChanged;

	UPROPERTY(BlueprintAssignable)
	FVTPauseChanged PauseChanged;
};
