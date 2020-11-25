// Fill out your copyright notice in the Description page of Project Settings.


#include "VTNetworkClient.h"

#include "TcpSocketConnection.h"
#include "PhoneticPhrase.h"
#include "HAL/UnrealMemory.h"

UVTNetworkClient* UVTNetworkClient::CreateAndConnect(
	FString Host,
	int32 Port,
	const FVTNetworkClientStatusChangedDelegate& OnConnect,
	const FVTNetworkClientStatusChangedDelegate& OnDisconnect
){
	UVTNetworkClient* Client = NewObject<UVTNetworkClient>();
	Client->Connect(Host, Port, OnConnect, OnDisconnect);

	return Client;
}

void UVTNetworkClient::Connect(
	FString Host,
	int32 Port,
	const FVTNetworkClientStatusChangedDelegate& OnConnect,
	const FVTNetworkClientStatusChangedDelegate& OnDisconnect
){
	UE_LOG(LogTemp, Log, TEXT("UVTNetworkClient: Connecting to %s:%d..."), *Host, Port);
	ConnectionState = EDeviceConnectionState::Connecting;

	if(Host == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("UVTNetworkClient: Bad or unspecified host"));
		return;
	}

	TcpClient = NewObject<ATcpSocketConnection>();

	FTcpSocketConnectDelegate tmpConnectDelegate;
	tmpConnectDelegate.BindDynamic(this, &UVTNetworkClient::OnConnected);
	ConnectedDelegate = OnConnect;

	FTcpSocketDisconnectDelegate tmpDisconnectDelegate;
	tmpDisconnectDelegate.BindDynamic(this, &UVTNetworkClient::OnDisconnected);
	DisconnectedDelegate = OnDisconnect;

	FTcpSocketReceivedMessageDelegate ReceivedDelegate;
	ReceivedDelegate.BindDynamic(this, &UVTNetworkClient::OnMessageReceived);

	TcpClient->Connect(Host, Port, tmpDisconnectDelegate, tmpConnectDelegate, ReceivedDelegate, ConnectionID);
}

void UVTNetworkClient::Disconnect()
{
	UE_LOG(LogTemp, Log, TEXT("UVTNetworkClient: Disconnecting..."));
	if(ConnectionState == EDeviceConnectionState::Connected)
	{
		TcpClient->Disconnect(ConnectionID);
	}
}

void UVTNetworkClient::OnConnected(int32 ConnId) {
	UE_LOG(LogTemp, Log, TEXT("UVTNetworkClient: Connected."));
	ConnectionState = EDeviceConnectionState::Connected;
	ConnectedDelegate.ExecuteIfBound();
}

void UVTNetworkClient::OnDisconnected(int32 ConnId) {
	UE_LOG(LogTemp, Log, TEXT("UVTNetworkClient: Disconnected."));

	ConnectionState = EDeviceConnectionState::Disconnected;
	DisconnectedDelegate.ExecuteIfBound();
}

void UVTNetworkClient::OnMessageReceived(int32 ConnID, TArray<uint8>& Message) {
}

void UVTNetworkClient::UploadPhrases(TArray<UPhoneticPhrase*> Phrases)
{
	UE_LOG(LogTemp, Log, TEXT("UVTNetworkClient: Setting sound bites of %d phrases"), Phrases.Num());
	if(ConnectionState != EDeviceConnectionState::Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("UVTNetworkClient: Not connected"));
		return;
	}
	UploadedPhrases.Reset();

	for(int32 Idx=0; Idx<Phrases.Num(); Idx++)
	{
		UPhoneticPhrase* Phrase = Phrases[Idx];
		if(!UploadedPhrases.Contains(Phrase))
		{
			UploadPhrase(Idx, Phrase);
		}
	}
}

void UVTNetworkClient::UploadPhrase(int32 ID, UPhoneticPhrase* Phrase)
{
	if(ConnectionState != EDeviceConnectionState::Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("UVTNetworkClient: Not connected"));
		return;
	}

	if(!IsValid(Phrase))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't send bad phrase"));
		return;
	}
	UploadedPhrases.Insert(Phrase, ID);

	UE_LOG(LogTemp, Log, TEXT("Sending sound bite at %d = %s"), ID, *(Phrase->WrittenText));
	TArray<uint8> Data;
	Data.Add(0x00);
	Data.Add(0x08);
	Data.Add((uint8_t)ID);

	for(int32 i=0; i<4; i++)
	{
		uint8_t b = Phrase->Period >> (i*8);
		Data.Add(b);
	}

	int32 SampleCount = Phrase->RawSamples.Num()/3;
	for(int i=0; i<4; i++)
	{
		uint8_t b = SampleCount >> (i*8);
		Data.Add(b);
	}

	for(int32 i=0; i<Phrase->RawSamples.Num(); i++){
		Data.Add(Phrase->RawSamples[i]);
	}

	UE_LOG(LogTemp, Log, TEXT("%d bytes"), Data.Num());
	TcpClient->SendData(ConnectionID, Data);
}

void UVTNetworkClient::PlayPhrase(UPhoneticPhrase* Phrase)
{
	if(ConnectionState != EDeviceConnectionState::Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("UVTNetworkClient: Not connected"));
		return;
	}

	if(!IsValid(Phrase))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't play invalid phrase"));
		return;
	}

	int32 Idx = UploadedPhrases.Find(Phrase);

	if(Idx == INDEX_NONE)
	{
		Idx = UploadedPhrases.Num();
		UploadPhrase(Idx, Phrase);
	}

	TArray<uint8> Data;
	Data.Add(0x00);
	Data.Add(0x09);
	Data.Add((uint8_t)Idx);
	TcpClient->SendData(ConnectionID, Data);
}
