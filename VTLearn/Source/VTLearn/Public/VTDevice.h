// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Misc/DateTime.h"
#include "Misc/Timespan.h"

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
class VTLEARN_API UVTDevice : public UObject, public FTickableGameObject
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
	void Ping();

	UFUNCTION(BlueprintCallable)
	void PlayPhrase(UPhoneticPhrase* Phrase);

	UFUNCTION(BlueprintCallable)
	void EnableActuator(int32 ActuatorID);

	UFUNCTION(BlueprintCallable)
	void DisableActuator(int32 ActuatorID);

	UFUNCTION(BlueprintCallable)
	void PulseActuator(int32 ActuatorID);

	UFUNCTION(BlueprintCallable)
	void DisableAll();

	UFUNCTION(BlueprintCallable)
	void BroadcastVibingStop();

	UPROPERTY(BlueprintAssignable)
	FVTDeviceVibingChanged DeviceVibingChanged;

	UPROPERTY(BlueprintAssignable)
	FVTDeviceConnectedChanged DeviceConnectedChanged;

	#pragma region Interface functions

  	UFUNCTION(BlueprintCallable)
	virtual void Disconnect(){}

	bool Send(TArray<uint8> Data, bool bAutoRecover = true);

	virtual bool Send_impl(TArray<uint8> Data, bool bAutoRecover = true)
	{
		return false;
	}

	virtual void Receive()
	{
	}

//	void ResetPingTimer();

	TArray<uint8> ReceiveBuffer;
	virtual void HandleMessageInBuffer();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual FString ToString()
	{
		return TEXT("");
	}

	#pragma endregion

	#pragma region FTickableGameObject functions
		void Tick(float DeltaTime) override;
		bool IsTickable() const override;
		bool IsTickableInEditor() const override;
		bool IsTickableWhenPaused() const override;

		TStatId GetStatId() const override;
		UWorld* GetWorld() const override;
	#pragma endregion

protected:
	FVTDeviceConnectionChangedDelegate ConnectedDelegate;
	FVTDeviceConnectionChangedDelegate DisconnectedDelegate;

	FTimerHandle VibingStateTimerHandle;

	FDateTime LastPingTimestamp;
	FDateTime LastBiteTimestamp;

	FTimespan CurrentBiteDuration;
};
