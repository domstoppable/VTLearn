// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VTDevice.h"

#include "PhoneticPhrase.h"
#include "TcpSocketConnection.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VTTCPDevice.generated.h"

//DECLARE_DYNAMIC_DELEGATE(FVTNetworkClientStatusChangedDelegate);

UCLASS(BlueprintType)
class VTLEARN_API UVTTCPDevice : public UVTDevice
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="VTT", meta=(AutoCreateRefTerm = "OnConnect, OnDisconnect"))
	static UVTTCPDevice* ConnectToTCPDevice(
		FString Host,
		int32 Port,
		const FVTDeviceConnectionChangedDelegate& OnConnect,
		const FVTDeviceConnectionChangedDelegate& OnDisconnect
	);

  	UFUNCTION(BlueprintCallable, Category="VTT", meta=(AutoCreateRefTerm = "OnConnect, OnDisconnect"))
	void Connect(
		FString Host,
		int32 Port,
		const FVTDeviceConnectionChangedDelegate& OnConnect,
		const FVTDeviceConnectionChangedDelegate& OnDisconnect
	);

	UFUNCTION()
	void OnClientConnected(int32 ConnID);

	UFUNCTION()
	void OnClientDisconnected(int32 ConnID);

	UFUNCTION()
	void OnMessageReceived(int32 ConnID, TArray<uint8>& Message);


	virtual void Disconnect() override;
	virtual bool Send(TArray<uint8> Data, bool bAutoRecover) override;

protected:

	UPROPERTY()
	int32 ConnectionID;

	UPROPERTY()
	ATcpSocketConnection* TcpClient;
};
