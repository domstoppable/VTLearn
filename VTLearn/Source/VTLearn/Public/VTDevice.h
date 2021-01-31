// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "TimerManager.h"

#include "VTDevice.generated.h"

UENUM(BlueprintType)
enum class EDeviceConnectionState : uint8
{
	Disconnected,
	Connecting,
	Connected
};

DECLARE_DYNAMIC_DELEGATE(FVTDeviceConnectionChangedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVTDeviceVibingChanged, bool, bIsVibing);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVTDeviceConnectedChanged, EDeviceConnectionState, ConnectionState);

UCLASS(BlueprintType)
class VTLEARN_API UVTDevice : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UObject* WorldContextObject;

	UPROPERTY(BlueprintReadOnly)
	EDeviceConnectionState ConnectionState;

	UPROPERTY(BlueprintReadOnly)
	TArray<UPhoneticPhrase*> UploadedPhrases;

	UFUNCTION(BlueprintCallable)
	void UploadPhrases(TArray<UPhoneticPhrase*> Phrases);

	UFUNCTION()
	void OnConnected();

	UFUNCTION()
	void OnDisconnected();

	UFUNCTION(BlueprintCallable)
	void UploadPhrase(int32 ID, UPhoneticPhrase* Phrase);

	UFUNCTION(BlueprintCallable)
	void PlayPhrase(UPhoneticPhrase* Phrase);

	UFUNCTION(BlueprintCallable)
	void BroadcastVibingStop();

	UPROPERTY(BlueprintAssignable)
	FVTDeviceVibingChanged DeviceVibingChanged;

	UPROPERTY(BlueprintAssignable)
	FVTDeviceConnectedChanged DeviceConnectedChanged;

	#pragma region Interface functions

  	UFUNCTION(BlueprintCallable)
	virtual void Disconnect(){}

	virtual bool Send(TArray<uint8> Data){
		return false;
	}

	#pragma endregion

protected:
	FVTDeviceConnectionChangedDelegate ConnectedDelegate;
	FVTDeviceConnectionChangedDelegate DisconnectedDelegate;

	FTimerHandle VibingStateTimerHandle;

};
