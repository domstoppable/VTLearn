#pragma once

#define BOOST_ALL_NO_LIB // https://stackoverflow.com/questions/16663302/error-lnk1104-cannot-open-file-libboost-serialization-vc110-mt-gd-1-53-lib

THIRD_PARTY_INCLUDES_START
#pragma push_macro("CONSTEXPR")
#undef CONSTEXPR
#pragma push_macro("dynamic_cast")
#undef dynamic_cast
#pragma push_macro("check")
#undef check
#pragma push_macro("PI")
#undef PI
#pragma push_macro("TEXT")
#undef TEXT

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#pragma pop_macro("TEXT")
#pragma pop_macro("PI")
#pragma pop_macro("check")
#pragma pop_macro("dynamic_cast")
#pragma pop_macro("CONSTEXPR")

THIRD_PARTY_INCLUDES_END

#include "Misc/Paths.h"

#include "CoreMinimal.h"
#include "VTDevice.h"
#include "VTSerialDevice.generated.h"

USTRUCT(BlueprintType)
struct FSerialPortInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Serial)
	FString Device;

	UPROPERTY(BlueprintReadOnly, Category=Serial)
	FString VendorID;

	UPROPERTY(BlueprintReadOnly, Category=Serial)
	FString ProductID;

	UPROPERTY(BlueprintReadOnly, Category=Serial)
	FString Manufacturer;

	UPROPERTY(BlueprintReadOnly, Category=Serial)
	FString Product;
};

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
	virtual bool Send(TArray<uint8> Data, bool bAutoRecover=true);
	virtual FString ToString() override;

	UFUNCTION()
	bool RecoverConnection();

	UFUNCTION(BlueprintCallable, Category="VTT")
	static TArray<FSerialPortInfo> GetSerialPorts();

	UFUNCTION(BlueprintPure, BlueprintCallable)
	static FString SerialPortToHumanReadable(FSerialPortInfo PortInfo)
	{
		FString DeviceString = FString::Printf(
			TEXT("[%s] %s - %s"),
			*FPaths::GetCleanFilename(PortInfo.Device),
			*PortInfo.Manufacturer,
			*PortInfo.Product
		);

		if(DeviceString.Len() > 40){
			DeviceString = DeviceString.Mid(0, 37) + "...";
		}

		return DeviceString;
	}

	UFUNCTION(BlueprintPure, BlueprintCallable)
	static bool IsPreferredDevice(FSerialPortInfo PortInfo)
	{
		return PortInfo.VendorID == "10c4" && PortInfo.ProductID == "ea60";
	}

	UPROPERTY(BlueprintReadOnly)
	FString LastPort;

	UPROPERTY(BlueprintReadOnly)
	int32 LastBaud;


protected:

	boost::asio::io_service io;
	boost::asio::serial_port* port = nullptr;

};
