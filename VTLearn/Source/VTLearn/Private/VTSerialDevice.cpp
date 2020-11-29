// Fill out your copyright notice in the Description page of Project Settings.

#include "VTSerialDevice.h"
#include "VTDevice.h"

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

	port = new boost::asio::serial_port(io, PortName);
	port->set_option(boost::asio::serial_port_base::baud_rate(Baud));

	ConnectedDelegate = OnConnect;
	DisconnectedDelegate = OnDisconnect;

	OnConnected();
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
	char data[Data.Num()];
	for(int32 i=0; i<Data.Num(); i++)
	{
		data[i] = (char)(Data[i]);
	}

	boost::asio::write(*port, boost::asio::buffer(data, Data.Num()));
}