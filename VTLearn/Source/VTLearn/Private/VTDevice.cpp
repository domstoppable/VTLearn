#include "VTDevice.h"

#include "Data/PsydekickData.h"
#include "PhoneticPhrase.h"
#include "TimerManager.h"
#include "VTGameInstance.h"

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
	UploadedPhrases.Empty();

	UE_LOG(LogTemp, Log, TEXT("UVTDevice: Connected (%s)"), *ToString());
	ConnectionState = EDeviceConnectionState::Connected;
	ConnectedDelegate.ExecuteIfBound();

	DeviceConnectedChanged.Broadcast(ConnectionState);

	if(IsValid(WorldContextObject))
	{
		UE_LOG(LogTemp, Log, TEXT("World context object is: %s"), *WorldContextObject->GetName());
		UWorld* World = WorldContextObject->GetWorld();
		if(IsValid(World))
		{
			World->GetTimerManager().SetTimer(PingTimerHandle, this, &UVTDevice::Ping, 0.5f, true, 0.5f);
		}
	}
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

void UVTDevice::Ping()
{
	if(ConnectionState != EDeviceConnectionState::Connected)
	{
		UE_LOG(LogTemp, Warning, TEXT("UVTDevice: (Ping) Not connected"));
		if(IsValid(WorldContextObject))
		{
			UWorld* World = WorldContextObject->GetWorld();
			if(IsValid(World))
			{
				World->GetTimerManager().ClearTimer(PingTimerHandle);
			}
		}

		return;
	}

	UE_LOG(LogTemp, Log, TEXT("UVTDevice: Ping"));

	TArray<uint8> Data;
	Data.Add(0x00);
	Data.Add(0xFE);

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

	UploadPhrase(0, Phrase);

	TArray<uint8> Data;
	Data.Add(0x00);
	Data.Add(0x09);
	Data.Add(0x00);

	UVTGameInstance* GameInstance = UVTGameInstance::GetVTGameInstance(WorldContextObject);

	bool bStimulusSendOk = Send(Data);
	if(bStimulusSendOk)
	{
		UWorld* World = WorldContextObject->GetWorld();
		if(IsValid(World))
		{
			// @TODO: Don't broadcast if paused, since the broadcast-stop timer won't start until unpaused?
			DeviceVibingChanged.Broadcast(true);

			float Duration = ((float)(Phrase->GetDurationInMS())) / 1000.0f;
			World->GetTimerManager().SetTimer(VibingStateTimerHandle, this, &UVTDevice::BroadcastVibingStop, Duration, false, Duration);
		}
	}

	GameInstance->LogStimulus(Phrase, bStimulusSendOk);
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
	}else{
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
