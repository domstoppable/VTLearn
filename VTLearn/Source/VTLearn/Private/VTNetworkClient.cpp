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
	UE_LOG(LogTemp, Log, TEXT("UVTNetworkClient: Connected to server."));

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
	TcpClient->Disconnect(ConnectionID);
}

void UVTNetworkClient::OnConnected(int32 ConnId) {
	UE_LOG(LogTemp, Log, TEXT("UVTNetworkClient: Connected to server."));
	ConnectedDelegate.ExecuteIfBound();
}

void UVTNetworkClient::OnDisconnected(int32 ConnId) {
	UE_LOG(LogTemp, Log, TEXT("UVTNetworkClient: OnDisconnected."));
	DisconnectedDelegate.ExecuteIfBound();
}

void UVTNetworkClient::OnMessageReceived(int32 ConnID, TArray<uint8>& Message) {
	/*
	UE_LOG(LogTemp, Log, TEXT("Log: Received message."));
  	// In this example, we always encode messages a certain way:
  	// The first 4 bytes contain the length of the rest of the message.
  	while (Message.Num() != 0) {
		// read expected length
		int32 msgLength = Message_ReadInt(Message);
		if (msgLength == -1) // when we can't read 4 bytes, the method returns -1
			return;
		TArray<uint8> yourMessage;
		// read the message itself
		if (!Message_ReadBytes(msgLength, Message, yourMessage)) {
			// If it couldn't read expected number of bytes, something went wrong.
			// Print a UE_LOG here, while your project is in development.
			continue;
		}
		// If the message was read, then treat "yourMessage" here!
		// ...
		// And then we go back to the "while", because we may have received multiple messages in a frame,
		// so they all have to be read.
  	}
	  */
}

void UVTNetworkClient::UploadPhrases(TArray<UPhoneticPhrase*> Phrases)
{
	UE_LOG(LogTemp, Log, TEXT("UVTNetworkClient: Setting sound bites of %d phrases"), Phrases.Num());
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
	if(!IsValid(Phrase))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't play invalid phrase"));
		return;
	}

	int32 Idx = UploadedPhrases.Find(Phrase);

	if(Idx == INDEX_NONE)
	{
		UploadPhrase(9, Phrase);
		Idx = 9;
	}

	TArray<uint8> Data;
	Data.Add(0x00);
	Data.Add(0x09);
	Data.Add((uint8_t)Idx);
	TcpClient->SendData(ConnectionID, Data);
}
