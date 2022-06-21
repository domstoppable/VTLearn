#include "VTDevice.h"

#include "Data/PsydekickData.h"
#include "PhoneticPhrase.h"
#include "TimerManager.h"
#include "VTGameInstance.h"

#include "Misc/DateTime.h"
#include "Misc/Timespan.h"

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

void UVTDevice::OnConnected()
{
	UploadedPhrases.Empty();

	UE_LOG(LogTemp, Log, TEXT("UVTDevice: Connected (%s)"), *ToString());
	ConnectionState = EDeviceConnectionState::Connected;
	ConnectedDelegate.ExecuteIfBound();

	DeviceConnectedChanged.Broadcast(ConnectionState);
}

bool UVTDevice::Send(TArray<uint8> Data, bool bAutoRecover)
{
	if(Data.Num() > 1 && Data[1] != 0x0C){
		UE_LOG(LogTemp, Log, TEXT("UVTDevice::Send %d (%d bytes)"), Data[1], Data.Num());
	}
	bool bResult = Send_impl(Data, bAutoRecover);

	LastPingTimestamp = FDateTime::Now();

	return bResult;
}

void UVTDevice::OnDisconnected() {
	UE_LOG(LogTemp, Log, TEXT("UVTDevice: Disconnected (%s)"), *ToString());
	UploadedPhrases.Empty();

	ConnectionState = EDeviceConnectionState::Disconnected;
	DisconnectedDelegate.ExecuteIfBound();
	DeviceConnectedChanged.Broadcast(ConnectionState);
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

	int32 SampleCount = Phrase->RawSamples.Num()/3;
	UE_LOG(LogTemp, Log, TEXT("Sending sound bite at %d = %s, period = %d, samples = %d"), ID, *(Phrase->WrittenText), Phrase->Period, SampleCount);
	TArray<uint8> Data;
	Data.Add(0x00);
	Data.Add(0x08);
	Data.Add((uint8_t)ID);

	for(int32 i=0; i<4; i++)
	{
		uint8_t b = Phrase->Period >> (i*8);
		Data.Add(b);
	}

	for(int i=0; i<4; i++)
	{
		uint8_t b = SampleCount >> (i*8);
		Data.Add(b);
	}

	for(int32 i=0; i<Phrase->RawSamples.Num(); i++){
		Data.Add(Phrase->RawSamples[i]);
	}

	Send(Data);
}

void UVTDevice::Ping()
{
	TArray<uint8> Data;
	Data.Add(0x00);
	Data.Add(0x0C);

	Send(Data);

	//Receive();
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

	FDateTime Now = FDateTime::Now();

	FTimespan Delta = Now - LastBiteTimestamp;
	if(Delta < CurrentBiteDuration){
		UE_LOG(LogTemp, Log, TEXT("UVTDevice::PlayPhrase Ignoring phrase play request - too fast!"));
		return;
	}

	UploadPhrase(0, Phrase);

	TArray<uint8> Data;
	Data.Add(0x00);
	Data.Add(0x09);
	Data.Add(0x00);

	UWorld* World = GetWorld();
	if(IsValid(World))
	{
		UVTGameInstance* GameInstance = UVTGameInstance::GetVTGameInstance(World);

		bool bStimulusSendOk = Send(Data);
		if(bStimulusSendOk)
		{
			DeviceVibingChanged.Broadcast(true);
			CurrentBiteDuration = FTimespan::FromMilliseconds(Phrase->GetDurationInMS()) + FTimespan::FromMilliseconds(750);
			LastBiteTimestamp = Now;
		}
		GameInstance->LogStimulus(Phrase, bStimulusSendOk);
	}
}

void UVTDevice::EnableActuator(int32 ActuatorID)
{
	if(ConnectionState != EDeviceConnectionState::Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("UVTDevice: Not connected"));
		return;
	}

	TArray<uint8> Data;
	Data.Add(0x00);
	Data.Add(0x0A);
	Data.Add(ActuatorID);
	Data.Add(254);

	if(Send(Data))
	{
		UE_LOG(LogTemp, Log, TEXT("Sent ACTIVATE to %d"), ActuatorID);
	}else{
		UE_LOG(LogTemp, Log, TEXT("FAILED: ACTIVATE %d"), ActuatorID);
	}
}

void UVTDevice::DisableActuator(int32 ActuatorID)
{
	if(ConnectionState != EDeviceConnectionState::Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("UVTDevice: Not connected"));
		return;
	}

	TArray<uint8> Data;
	Data.Add(0x00);
	Data.Add(0x0A);
	Data.Add(ActuatorID);
	Data.Add(0);

	if(Send(Data))
	{
		UE_LOG(LogTemp, Log, TEXT("Sent DEactivate to %d"), ActuatorID);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("FAILED: DEactivate %d"), ActuatorID);
	}

}

void UVTDevice::PulseActuator(int32 ActuatorID)
{
	if(ConnectionState != EDeviceConnectionState::Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("UVTDevice: Not connected"));
		return;
	}

	TArray<uint8> Data;
	Data.Add(0x00);
	Data.Add(0x0B);
	Data.Add(ActuatorID);

	Send(Data);
}

void UVTDevice::DisableAll()
{
	if(ConnectionState != EDeviceConnectionState::Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("UVTDevice: Not connected"));
		return;
	}

	TArray<uint8> Data;
	Data.Add(0x00);
	Data.Add(0x07);

	if(Send(Data))
	{
		UE_LOG(LogTemp, Log, TEXT("Sent STOP"));
	}else{
		UE_LOG(LogTemp, Log, TEXT("FAILED: send STOP"));
	}

}

void UVTDevice::BroadcastVibingStop()
{
	DeviceVibingChanged.Broadcast(false);
}

void UVTDevice::HandleMessageInBuffer()
{
	char CharBuffer[ReceiveBuffer.Num()+1];
	int i = 0;
	for(uint8 Byte : ReceiveBuffer)
	{
		CharBuffer[i++] = Byte;
	}
	CharBuffer[i++] = '\0';

	FString Message(CharBuffer);

	if(Message != "ok")
	{
		UE_LOG(LogTemp, Warning, TEXT("Device data: %s"), *Message);
	}
	ReceiveBuffer.Empty();
}

void UVTDevice::Tick(float DeltaTime)
{
	FDateTime Now = FDateTime::Now();
	if(ConnectionState == EDeviceConnectionState::Connected)
	{
		FTimespan Delta = Now - LastPingTimestamp;
		if(Delta > FTimespan::FromSeconds(1))
		{
			Ping();
		}
	}

	FTimespan Delta = Now - LastBiteTimestamp;
	if(!CurrentBiteDuration.IsZero() && Delta > CurrentBiteDuration){
		BroadcastVibingStop();
		CurrentBiteDuration = FTimespan::Zero();
	}
}

bool UVTDevice::IsTickable() const
{
	return true;
}

bool UVTDevice::IsTickableInEditor() const
{
	return false;
}

bool UVTDevice::IsTickableWhenPaused() const
{
	return true;
}

TStatId UVTDevice::GetStatId() const
{
	return TStatId();
}

UWorld* UVTDevice::GetWorld() const
{
	return GetOuter()->GetWorld();
}