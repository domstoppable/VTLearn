// Fill out your copyright notice in the Description page of Project Settings.

#include "VTSerialDevice.h"

#include "VTDevice.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

UVTSerialDevice* UVTSerialDevice::ConnectToSerialDevice(
	FString Port,
	int32 Baud,
	const FVTDeviceConnectionChangedDelegate& OnConnect,
	const FVTDeviceConnectionChangedDelegate& OnDisconnect
){
	UVTSerialDevice* Device = NewObject<UVTSerialDevice>();
	Device->Connect(Port, Baud, OnConnect, OnDisconnect);

	return Device;
}

void UVTSerialDevice::Connect(
	FString Port,
	int32 Baud,
	const FVTDeviceConnectionChangedDelegate& OnConnect,
	const FVTDeviceConnectionChangedDelegate& OnDisconnect
){
	std::string PortName = std::string(TCHAR_TO_UTF8(*Port));
	try{
		ConnectionState = EDeviceConnectionState::Connecting;

		port = new boost::asio::serial_port(io, PortName);
		port->set_option(boost::asio::serial_port_base::baud_rate(Baud));

		ConnectedDelegate = OnConnect;
		DisconnectedDelegate = OnDisconnect;

		ConnectionState = EDeviceConnectionState::Connected;
		OnConnected();
	}
	catch(std::exception const& exception)
	{
		ConnectionState = EDeviceConnectionState::Disconnected;
		FString Message(exception.what());
		UE_LOG(LogTemp, Warning, TEXT("Failed to connect to serial device %s @%d : %s"), *Port, Baud, *Message);
	}
}

void UVTSerialDevice::BeginDestroy()
{
	Super::BeginDestroy();

	Disconnect();
	delete port;
	port = nullptr;
}


void UVTSerialDevice::Disconnect()
{
}

void UVTSerialDevice::Send(TArray<uint8> Data)
{
	char* data = new char[Data.Num()];
	for(int32 i=0; i<Data.Num(); i++)
	{
		data[i] = (char)(Data[i]);
	}

	try{
		boost::asio::write(*port, boost::asio::buffer(data, Data.Num()));
	}
	catch(std::exception const& exception)
	{
		FString Message(exception.what());
		UE_LOG(LogTemp, Warning, TEXT("Failed to write %d bytes to serial device: %s"), Data.Num(), *Message);
	}

	delete[] data;
}


TArray<FSerialPortInfo> UVTSerialDevice::GetSerialPorts()
{
	TArray<FSerialPortInfo> Ports;

	#if defined(WIN32) || defined(_WIN32)
		wchar_t lpTargetPath[5000];

		for (int i=0; i<256; i++)
		{
			std::string str = "COM" + std::to_string(i);
			std::wstring stemp = std::wstring(str.begin(), str.end());
			LPCWSTR sw = stemp.c_str();

			DWORD test = QueryDosDevice(sw, lpTargetPath, 5000);

			if (test != 0)
			{
				Ports.Add(FString(str.c_str()));
			}
		}
	#else
		class FDeviceNodeVisitor : public IPlatformFile::FDirectoryVisitor
		{
		public:
			FDeviceNodeVisitor() {}

			virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory)
			{
				if (!bIsDirectory)
				{
					FString Path(FilenameOrDirectory);

					if (Path.Contains("ttyUSB"))
					{
						DevicePaths.Add(Path);
					}
				}
				return true;
			}

			TArray<FString> DevicePaths;
		};

		const FString DeviceFolder = TEXT("/dev/");

		FDeviceNodeVisitor Visitor;
		FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*DeviceFolder, Visitor);
		for(FString Path : Visitor.DevicePaths)
		{
			UE_LOG(LogTemp, Log, TEXT("Examining %s"), *Path);
			FSerialPortInfo PortInfo;
			PortInfo.Device = Path;

			char sympath[PATH_MAX];
			sprintf(sympath, "/sys/class/tty/%s/device", TCHAR_TO_ANSI(*FPaths::GetCleanFilename(Path)));
			char abspath[PATH_MAX];
			char *res = realpath(sympath, abspath);

			FString DeviceInfoPath = FString(abspath) + TEXT("/../../");
			UE_LOG(LogTemp, Log, TEXT("\tdevice info path '%s'"), *DeviceInfoPath);

			TArray<FString> Tmp;

			FFileHelper::LoadANSITextFileToStrings(*(DeviceInfoPath + "idVendor"), &IFileManager::Get(), Tmp);
			if(Tmp.Num() > 0) PortInfo.VendorID = Tmp[0];

			Tmp.Reset();
			FFileHelper::LoadANSITextFileToStrings(*(DeviceInfoPath + "idProduct"), &IFileManager::Get(), Tmp);
			if(Tmp.Num() > 0) PortInfo.ProductID = Tmp[0];

			Tmp.Reset();
			FFileHelper::LoadANSITextFileToStrings(*(DeviceInfoPath + "manufacturer"), &IFileManager::Get(), Tmp);
			if(Tmp.Num() > 0) PortInfo.Manufacturer = Tmp[0];

			Tmp.Reset();
			FFileHelper::LoadANSITextFileToStrings(*(DeviceInfoPath + "product"), &IFileManager::Get(), Tmp);
			if(Tmp.Num() > 0) PortInfo.Product = Tmp[0];

			UE_LOG(LogTemp, Log, TEXT("\tdevice ids %s:%s"), *PortInfo.VendorID, *PortInfo.ProductID);
			UE_LOG(LogTemp, Log, TEXT("\tmanufacturer %s"), *PortInfo.Manufacturer);
			UE_LOG(LogTemp, Log, TEXT("\tproduct %s"), *PortInfo.Product);

			Ports.Add(PortInfo);
		}

	#endif

	return Ports;
}
