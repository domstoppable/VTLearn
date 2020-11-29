// Fill out your copyright notice in the Description page of Project Settings.


#include "VTDevice.h"

#include "PhoneticPhrase.h"

void UVTDevice::UploadPhrases(TArray<UPhoneticPhrase*> Phrases)
{
	UE_LOG(LogTemp, Log, TEXT("UVTDevice: Setting sound bites of %d phrases"), Phrases.Num());
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

void UVTDevice::OnConnected() {
	UE_LOG(LogTemp, Log, TEXT("UVTDevice: Connected."));
	ConnectionState = EDeviceConnectionState::Connected;
	ConnectedDelegate.ExecuteIfBound();
}

void UVTDevice::OnDisconnected() {
	UE_LOG(LogTemp, Log, TEXT("UVTDevice: Disconnected."));

	ConnectionState = EDeviceConnectionState::Disconnected;
	DisconnectedDelegate.ExecuteIfBound();
}

void UVTDevice::UploadPhrase(int32 ID, UPhoneticPhrase* Phrase)
{
	if(ConnectionState != EDeviceConnectionState::Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("UVTDevice: Not connected"));
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
	Send(Data);
}

void UVTDevice::PlayPhrase(UPhoneticPhrase* Phrase)
{
	if(ConnectionState != EDeviceConnectionState::Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("UVTDevice: Not connected"));
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

	Send(Data);
}
