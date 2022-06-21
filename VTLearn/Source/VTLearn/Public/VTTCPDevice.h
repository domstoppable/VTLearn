// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "VTDevice.h"

#include "PhoneticPhrase.h"
#include "TcpSocketConnection.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VTTCPDevice.generated.h"

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
		const FVTDeviceConnectionChangedDelegate& OnDisconnect,
		UObject* InOuter
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
	virtual bool Send_impl(TArray<uint8> Data, bool bAutoRecover) override;
	virtual void Receive() override;

protected:

	UPROPERTY()
	int32 ConnectionID;

	UPROPERTY()
	ATcpSocketConnection* TcpClient;
};
