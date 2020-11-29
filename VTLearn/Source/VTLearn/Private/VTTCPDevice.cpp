// Fill out your copyright notice in the Description page of Project Settings.

#include "VTTCPDevice.h"

#include "VTDevice.h"

#include "TcpSocketConnection.h"
#include "HAL/UnrealMemory.h"

UVTTCPDevice* UVTTCPDevice::ConnectToTCPDevice(
	FString Host,
	int32 Port,
	const FVTDeviceConnectionChangedDelegate& OnConnect,
	const FVTDeviceConnectionChangedDelegate& OnDisconnect
){
	UVTTCPDevice* Client = NewObject<UVTTCPDevice>();
	Client->Connect(Host, Port, OnConnect, OnDisconnect);

	return Client;
}

void UVTTCPDevice::Connect(
	FString Host,
	int32 Port,
	const FVTDeviceConnectionChangedDelegate& OnConnect,
	const FVTDeviceConnectionChangedDelegate& OnDisconnect
){
	UE_LOG(LogTemp, Log, TEXT("UVTTCPDevice: Connecting to %s:%d..."), *Host, Port);
	ConnectionState = EDeviceConnectionState::Connecting;

	if(Host == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("UVTTCPDevice: Bad or unspecified host"));
		return;
	}

	TcpClient = NewObject<ATcpSocketConnection>();

	FTcpSocketConnectDelegate tmpConnectDelegate;
	tmpConnectDelegate.BindDynamic(this, &UVTTCPDevice::OnClientConnected);
	ConnectedDelegate = OnConnect;

	FTcpSocketDisconnectDelegate tmpDisconnectDelegate;
	tmpDisconnectDelegate.BindDynamic(this, &UVTTCPDevice::OnClientDisconnected);
	DisconnectedDelegate = OnDisconnect;

	FTcpSocketReceivedMessageDelegate ReceivedDelegate;
	ReceivedDelegate.BindDynamic(this, &UVTTCPDevice::OnMessageReceived);

	TcpClient->Connect(Host, Port, tmpDisconnectDelegate, tmpConnectDelegate, ReceivedDelegate, ConnectionID);
}

void UVTTCPDevice::Disconnect()
{
	UE_LOG(LogTemp, Log, TEXT("UVTTCPDevice: Disconnecting..."));
	if(ConnectionState == EDeviceConnectionState::Connected)
	{
		TcpClient->Disconnect(ConnectionID);
	}
}

void UVTTCPDevice::OnMessageReceived(int32 ConnID, TArray<uint8>& Message)
{
}

void UVTTCPDevice::OnClientConnected(int32 ConnID)
{
	OnConnected();
}

void UVTTCPDevice::OnClientDisconnected(int32 ConnID)
{
	OnDisconnected();
}

void UVTTCPDevice::Send(TArray<uint8> Data)
{
	TcpClient->SendData(ConnectionID, Data);
}
