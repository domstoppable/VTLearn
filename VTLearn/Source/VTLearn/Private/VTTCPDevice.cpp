// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VTTCPDevice.h"

#include "VTDevice.h"

#include "TcpSocketConnection.h"
#include "HAL/UnrealMemory.h"

UVTTCPDevice* UVTTCPDevice::ConnectToTCPDevice(
	FString Host,
	int32 Port,
	const FVTDeviceConnectionChangedDelegate& OnConnect,
	const FVTDeviceConnectionChangedDelegate& OnDisconnect,
	UObject* InOuter
){
	UVTTCPDevice* Client = NewObject<UVTTCPDevice>(InOuter);
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

bool UVTTCPDevice::Send_impl(TArray<uint8> Data, bool bAutoRecover)
{
	return TcpClient->SendData(ConnectionID, Data);
}

void UVTTCPDevice::Receive()
{
	//@TODO: implement TCP reads
}