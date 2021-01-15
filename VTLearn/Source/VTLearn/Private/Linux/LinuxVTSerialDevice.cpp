#include "VTSerialDevice.h"

#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

#include <limits.h>
#include <stdlib.h>

TArray<FSerialPortInfo> UVTSerialDevice::GetSerialPorts()
{
	TArray<FSerialPortInfo> Ports;

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

		Ports.Add(PortInfo);
	}

	return Ports;
}
