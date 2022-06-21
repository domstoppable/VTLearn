// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VTSerialDevice.h"

#include "VTDevice.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

UVTSerialDevice* UVTSerialDevice::ConnectToSerialDevice(
	FString Port,
	int32 Baud,
	const FVTDeviceConnectionChangedDelegate& OnConnect,
	const FVTDeviceConnectionChangedDelegate& OnDisconnect,
	UObject* InOuter
){
	UE_LOG(LogTemp, Log, TEXT("UVTSerialDevice::ConnectToSerialDevice"));

	UVTSerialDevice* Device = NewObject<UVTSerialDevice>(InOuter);

	Device->Connect(Port, Baud, OnConnect, OnDisconnect);

	return Device;
}

void UVTSerialDevice::Connect(
	FString Port,
	int32 Baud,
	const FVTDeviceConnectionChangedDelegate& OnConnect,
	const FVTDeviceConnectionChangedDelegate& OnDisconnect
){
	UE_LOG(LogTemp, Log, TEXT("UVTSerialDevice::Connect"));

	std::string PortName = std::string(TCHAR_TO_UTF8(*Port));
	try{
		ConnectionState = EDeviceConnectionState::Connecting;

		port = new boost::asio::serial_port(io, PortName);
		port->set_option(boost::asio::serial_port_base::baud_rate(Baud));

		ConnectedDelegate = OnConnect;
		DisconnectedDelegate = OnDisconnect;

		ConnectionState = EDeviceConnectionState::Connected;

		LastPort = Port;
		LastBaud = Baud;
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
	UE_LOG(LogTemp, Log, TEXT("UVTSerialDevice::BeginDestroy"));
	Super::BeginDestroy();
	Disconnect();
}

void UVTSerialDevice::Disconnect()
{
	UE_LOG(LogTemp, Log, TEXT("UVTSerialDevice::Disconnect"));
	if(port)
	{
		port->close();
		delete port;
	}
	port = nullptr;
	ConnectionState = EDeviceConnectionState::Disconnected;
	OnDisconnected();
}

bool UVTSerialDevice::Send_impl(TArray<uint8> Data, bool bAutoRecover)
{
	bool Success = false;

	char* data = new char[Data.Num()];
	for(int32 i=0; i<Data.Num(); i++)
	{
		data[i] = (char)(Data[i]);
	}

	try{
		if(ConnectionState == EDeviceConnectionState::Connected)
		{
			boost::asio::write(*port, boost::asio::buffer(data, Data.Num()));
			Success = true;
		}
		else
		{
			throw std::exception();
		}
	}
	catch(std::exception const& exception)
	{
		FString Message(exception.what());
		UE_LOG(LogTemp, Warning, TEXT("Failed to write %d bytes to serial device: %s"), Data.Num(), *Message);
		if(bAutoRecover && RecoverConnection())
		{
			Success = Send(Data, false);
		}
		else
		{
			Disconnect();
		}
	}

	delete[] data;

	return Success;
}

void UVTSerialDevice::Receive()
{
	try{
		if(ConnectionState == EDeviceConnectionState::Connected)
		{
			boost::system::error_code ec;

			char Buffer[1024];
			size_t Length = boost::asio::read(
				*port,
				boost::asio::buffer(Buffer, 1024),
				boost::asio::transfer_at_least(1),
				ec
			);
			if(ec)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to read from serial device: %d"), ec.value());
				return;
			}

			for(int i=0; i<Length; i++){
				char Byte = Buffer[i];
				if(Byte == '\n')
				{
					HandleMessageInBuffer();
				}
				else
				{
					ReceiveBuffer.Add(Byte);
				}
			}
		}
		else
		{
			throw std::exception();
		}
	}
	catch(std::exception const& exception)
	{
		FString Message(exception.what());
		UE_LOG(LogTemp, Warning, TEXT("Failed to read from serial device: %s"), *Message);
	}
}

bool UVTSerialDevice::RecoverConnection()
{
	UE_LOG(LogTemp, Log, TEXT("Attempting to recover serial connection"));

	TArray<FSerialPortInfo> Ports = UVTSerialDevice::GetSerialPorts();
	for(FSerialPortInfo PortInfo : Ports)
	{
		if(UVTSerialDevice::IsPreferredDevice(PortInfo))
		{
			Connect(PortInfo.Device, LastBaud, ConnectedDelegate, DisconnectedDelegate);
			return ConnectionState == EDeviceConnectionState::Connected;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Connection recovery failed!"));
	return false;
}

FString UVTSerialDevice::ToString()
{
	return FString::Printf(TEXT("%s @ %d"), *LastPort, LastBaud);
}
