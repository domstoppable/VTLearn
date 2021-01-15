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
		// modified from https://aticleworld.com/get-com-port-of-usb-serial-device/
		HDEVINFO DeviceInfoSet = SetupDiGetClassDevs(NULL, L"USB", NULL, DIGCF_ALLCLASSES|DIGCF_PRESENT);
		if (DeviceInfoSet == INVALID_HANDLE_VALUE){
			UE_LOG(LogTemp, Warning, TEXT("Failed to load USB devices"));
			return Ports;
		}
		
		DWORD DeviceIndex = 0;
		SP_DEVINFO_DATA DeviceInfoData;
		BYTE szBuffer[1024] = {0};
		DEVPROPTYPE ulPropertyType;
		DWORD dwSize = 0;
		DWORD Error = 0;
		
		//Fills a block of memory with zeros
		ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
		DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		
		//Receive information about an enumerated device
		while (SetupDiEnumDeviceInfo(DeviceInfoSet, DeviceIndex, &DeviceInfoData))
		{
			//Retrieves a specified Plug and Play device property
			if (SetupDiGetDeviceRegistryProperty(
				DeviceInfoSet, &DeviceInfoData,
				SPDRP_HARDWAREID,
				&ulPropertyType, (BYTE*)szBuffer, sizeof(szBuffer), NULL
			))
			{
				HKEY hDeviceRegistryKey;
				//Get the key
				hDeviceRegistryKey = SetupDiOpenDevRegKey(DeviceInfoSet, &DeviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
				if (hDeviceRegistryKey == INVALID_HANDLE_VALUE)
				{
					Error = GetLastError();
					UE_LOG(LogTemp, Warning, TEXT("SetupDiOpenDevRegKey failed"));
					break;
				}
				else
				{
					// Read in the name of the port
					wchar_t pszPortName[64];
					DWORD dwSize2 = sizeof(pszPortName);
					DWORD dwType = 0;
					if(
						(RegQueryValueEx(hDeviceRegistryKey, L"PortName", NULL, &dwType, (LPBYTE) pszPortName, &dwSize2) == ERROR_SUCCESS)
						&& (dwType == REG_SZ)
					)
					{
						// Check if it really is a com port
						if( _tcsnicmp( pszPortName, _T("COM"), 3) == 0)
						{
							int nPortNr = _ttoi( pszPortName + 3 );
							if( nPortNr != 0 )
							{
								// it is a COM port, add it to the list
								FSerialPortInfo PortInfo;

								std::wstring tmpPortName(pszPortName);
								PortInfo.Device = FString(tmpPortName.c_str());

								// Parse out VID/PID
								std::wstring tmpDeviceID(reinterpret_cast<wchar_t*>(szBuffer), sizeof(szBuffer)/sizeof(wchar_t));
								FString DeviceIDString(tmpDeviceID.c_str());
								DeviceIDString.RemoveFromStart(TEXT("USB\\"));

								TArray<FString> Elements;
								DeviceIDString.ParseIntoArray(Elements, TEXT("&"));
								for(FString KeyValuePair : Elements)
								{
									TArray<FString> Bits;
									KeyValuePair.ParseIntoArray(Bits, TEXT("_"));
									if(Bits[0].Equals("VID"))
									{
										PortInfo.VendorID = Bits[1];
									}else if(Bits[0].Equals("PID"))
									{
										PortInfo.ProductID = Bits[1];
									}
								}

								// Get manufacturer
								if (SetupDiGetDeviceRegistryProperty(
									DeviceInfoSet, &DeviceInfoData,
									SPDRP_MFG,
									&ulPropertyType, (BYTE*)szBuffer, sizeof(szBuffer), NULL
								))
								{
									std::wstring tmpManufacturer(reinterpret_cast<wchar_t*>(szBuffer), sizeof(szBuffer)/sizeof(wchar_t));
									PortInfo.Manufacturer = FString(tmpManufacturer.c_str());
								}

								// Get product
								if (SetupDiGetDeviceRegistryProperty(
									DeviceInfoSet, &DeviceInfoData,
									SPDRP_FRIENDLYNAME,
									&ulPropertyType, (BYTE*)szBuffer, sizeof(szBuffer), NULL
								))
								{
									std::wstring tmpFriendlyName(reinterpret_cast<wchar_t*>(szBuffer), sizeof(szBuffer)/sizeof(wchar_t));
									FString FriendlyName(tmpFriendlyName.c_str());
									FriendlyName.RemoveFromStart(PortInfo.Manufacturer + " ");
									FriendlyName.RemoveFromEnd(" (" + PortInfo.Device + ")");

									PortInfo.Product = FriendlyName;
								}
								
								Ports.Add(PortInfo);
							}
						}
					}

					// Close the key now that we are finished with it
					RegCloseKey(hDeviceRegistryKey);
				}
			}else{
				UE_LOG(LogTemp, Warning, TEXT("Could not enumerating device %d"), DeviceIndex);

			}
			DeviceIndex++;
		}
		if (DeviceInfoSet)
		{
			SetupDiDestroyDeviceInfoList(DeviceInfoSet);
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
