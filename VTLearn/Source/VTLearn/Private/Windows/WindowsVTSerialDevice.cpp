#include "VTSerialDevice.h"

THIRD_PARTY_INCLUDES_START
/*
#pragma push_macro("CONSTEXPR")
#undef CONSTEXPR
#pragma push_macro("dynamic_cast")
#undef dynamic_cast
#pragma push_macro("check")
#undef check
#pragma push_macro("PI")
#undef PI
*/
#pragma push_macro("TEXT")
#undef TEXT

/*
// BOOST libs
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
*/

// platform includes
#include <string>
#include <Windows.h>
#include <setupapi.h>

#pragma pop_macro("TEXT")
/*
#pragma pop_macro("PI")
#pragma pop_macro("check")
#pragma pop_macro("dynamic_cast")
#pragma pop_macro("CONSTEXPR")
*/

THIRD_PARTY_INCLUDES_END



TArray<FSerialPortInfo> UVTSerialDevice::GetSerialPorts()
{
	TArray<FSerialPortInfo> Ports;

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

	return Ports;
}
