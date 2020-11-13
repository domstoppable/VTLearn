// Fill out your copyright notice in the Description page of Project Settings.

#pragma once



#include "GameFramework/Actor.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VTPlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVTControllerLivesChanged, int32, Lives);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVTControllerScoreChanged, int32, Score);

UCLASS()
class VTLEARN_API AVTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	AActor* HeldItem;

	UPROPERTY(BlueprintReadWrite)
	int32 Score;

	UPROPERTY(BlueprintReadWrite)
	int32 Lives = 5;

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

	UFUNCTION(BlueprintCallable)
	int32 IncreaseScore();

	UFUNCTION(BlueprintCallable)
	int32 DecreaseLives();

	UPROPERTY(BlueprintAssignable)
	FVTControllerScoreChanged ScoreChanged;

	UPROPERTY(BlueprintAssignable)
	FVTControllerLivesChanged LivesChanged;
};
