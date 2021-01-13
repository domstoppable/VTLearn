// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

THIRD_PARTY_INCLUDES_START
#pragma push_macro("CONSTEXPR")
#undef CONSTEXPR
#pragma push_macro("dynamic_cast")
#undef dynamic_cast
#pragma push_macro("check")
#undef check
#pragma push_macro("PI")
#undef PI

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#pragma pop_macro("PI")
#pragma pop_macro("check")
#pragma pop_macro("dynamic_cast")
#pragma pop_macro("CONSTEXPR")

THIRD_PARTY_INCLUDES_END

#include "CoreMinimal.h"
#include "VTDevice.h"
#include "VTSerialDevice.generated.h"

UCLASS()
class VTLEARN_API UVTSerialDevice : public UVTDevice
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="VTT", meta=(AutoCreateRefTerm = "OnConnect, OnDisconnect"))
	static UVTSerialDevice* ConnectToSerialDevice(
		FString Port,
		int32 Baud,
		const FVTDeviceConnectionChangedDelegate& OnConnect,
		const FVTDeviceConnectionChangedDelegate& OnDisconnect
	);

  	UFUNCTION(BlueprintCallable, Category="VTT", meta=(AutoCreateRefTerm = "OnConnect, OnDisconnect"))
	void Connect(
		FString Port,
		int32 Baud,
		const FVTDeviceConnectionChangedDelegate& OnConnect,
		const FVTDeviceConnectionChangedDelegate& OnDisconnect
	);

	virtual void BeginDestroy() override;
	virtual void Disconnect() override;
	virtual void Send(TArray<uint8> Data) override;

	UFUNCTION(BlueprintCallable, Category="VTT")
	static TArray<FString> GetSerialPorts();

protected:

	boost::asio::io_service io;
	boost::asio::serial_port* port = nullptr;

};
