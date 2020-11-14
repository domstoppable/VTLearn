// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VTNetworkClient.h"

#include "GameFramework/Actor.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VTPlayerController.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVTDeviceConnectionChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVTControllerLivesChanged, int32, Lives);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVTControllerScoreChanged, int32, Score);

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
	UVTNetworkClient* VTDevice;

	UPROPERTY(BlueprintReadOnly)
	AActor* HeldItem;

	UPROPERTY(BlueprintReadWrite)
	int32 Score;

	UPROPERTY(BlueprintReadWrite)
	int32 Lives = 5;

	UPROPERTY(BlueprintReadOnly)
	bool Paused = false;

	virtual void SetupInputComponent() override;

	void OnMoveUp(float Value);
	void OnMoveRight(float Value);
	void OnJump();
	void OnGrab();
	void OnInteract();

	UFUNCTION(BlueprintCallable)
	void TogglePause();

	virtual void Pause() override;

	UFUNCTION(BlueprintCallable)
	void ConnectToDevice(FString IP, int32 Port);

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

	UFUNCTION()
	void OnDeviceConnected();

	UFUNCTION()
	void OnDeviceDisconnected();

	UPROPERTY(BlueprintAssignable)
	FVTDeviceConnectionChanged DeviceConnected;

	UPROPERTY(BlueprintAssignable)
	FVTDeviceConnectionChanged DeviceDisconnected;

	UPROPERTY(BlueprintAssignable)
	FVTControllerScoreChanged ScoreChanged;

	UPROPERTY(BlueprintAssignable)
	FVTControllerLivesChanged LivesChanged;
};
