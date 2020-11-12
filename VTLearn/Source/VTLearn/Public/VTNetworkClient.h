// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PhoneticPhrase.h"
#include "TcpSocketConnection.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VTNetworkClient.generated.h"

DECLARE_DYNAMIC_DELEGATE(FVTNetworkClientStatusChangedDelegate);
/**
 *
 */
UCLASS(BlueprintType)
class VTLEARN_API UVTNetworkClient : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="VTT", meta=(AutoCreateRefTerm = "OnConnect, OnDisconnect"))
	static UVTNetworkClient* CreateAndConnect(
		FString Host,
		int32 Port,
		const FVTNetworkClientStatusChangedDelegate& OnConnect,
		const FVTNetworkClientStatusChangedDelegate& OnDisconnect
	);

  	UFUNCTION(BlueprintCallable, Category="VTT", meta=(AutoCreateRefTerm = "OnConnect, OnDisconnect"))
	void Connect(
		FString Host,
		int32 Port,
		const FVTNetworkClientStatusChangedDelegate& OnConnect,
		const FVTNetworkClientStatusChangedDelegate& OnDisconnect
	);

  	UFUNCTION(BlueprintCallable)
	void Disconnect();

	UFUNCTION()
	void OnConnected(int32 ConnID);

	UFUNCTION()
	void OnDisconnected(int32 ConnID);

	UFUNCTION()
	void OnMessageReceived(int32 ConnID, TArray<uint8>& Message);

	UFUNCTION(BlueprintCallable)
	void UploadPhrases(TArray<UPhoneticPhrase*> Phrases);

	UFUNCTION(BlueprintCallable)
	void UploadPhrase(int32 ID, UPhoneticPhrase* Phrase);

	UFUNCTION(BlueprintCallable)
	void PlayPhrase(UPhoneticPhrase* Phrase);

	UPROPERTY()
	int32 ConnectionID;

	UPROPERTY()
	ATcpSocketConnection* TcpClient;

	TArray<UPhoneticPhrase*> UploadedPhrases;

protected:
	FVTNetworkClientStatusChangedDelegate ConnectedDelegate;
	FVTNetworkClientStatusChangedDelegate DisconnectedDelegate;
};
