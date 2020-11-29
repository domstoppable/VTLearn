// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VTDevice.generated.h"

DECLARE_DYNAMIC_DELEGATE(FVTDeviceConnectionChangedDelegate);

UENUM(BlueprintType)
enum class EDeviceConnectionState : uint8
{
	Disconnected,
	Connecting,
	Connected
};

UCLASS(BlueprintType)
class VTLEARN_API UVTDevice : public UObject
{
	GENERATED_BODY()

public:
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

	#pragma region Interface functions

  	UFUNCTION(BlueprintCallable)
	virtual void Disconnect(){}

	virtual void Send(TArray<uint8> Data){}

	#pragma endregion

protected:
	FVTDeviceConnectionChangedDelegate ConnectedDelegate;
	FVTDeviceConnectionChangedDelegate DisconnectedDelegate;

};
