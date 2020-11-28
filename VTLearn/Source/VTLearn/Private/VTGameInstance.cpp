// Fill out your copyright notice in the Description page of Project Settings.


#include "VTGameInstance.h"

#include "Engine/Engine.h"


UVTGameInstance::UVTGameInstance()
{
	VTDevice = NewObject<UVTNetworkClient>();
	UE_LOG(LogTemp, Log, TEXT("GameInstance created"));
}

void UVTGameInstance::ConnectToDevice(FString IP, int32 Port)
{
	FVTNetworkClientStatusChangedDelegate ConnectDelegate;
	ConnectDelegate.BindDynamic(this, &UVTGameInstance::OnDeviceConnected);

	FVTNetworkClientStatusChangedDelegate DisconnectDelegate;
	DisconnectDelegate.BindDynamic(this, &UVTGameInstance::OnDeviceDisconnected);

	if(VTDevice->ConnectionState != EDeviceConnectionState::Connected)
	{
		VTDevice->Connect(IP, Port, ConnectDelegate, DisconnectDelegate);
	}
}

void UVTGameInstance::OnDeviceConnected()
{
	DeviceConnected.Broadcast();
}

void UVTGameInstance::OnDeviceDisconnected()
{
	DeviceDisconnected.Broadcast();
}

void UVTGameInstance::Shutdown()
{
	UE_LOG(LogTemp, Log, TEXT("GameInstance shutdown"));
	if(IsValid(VTDevice))
	{
		VTDevice->Disconnect();
	}
}

int32 ULevelStatus::StarCount()
{
	int32 Stars;
	for(Stars=0; Stars<LevelConfig.StarThresholds.Num(); Stars++)
	{
		if(LevelConfig.StarThresholds[Stars] > HighScore)
		{
			break;
		}
	}

	return Stars;
}

ULevelStatus* ULevelStatus::MakeLevelStatus(FLevelConfig InLevelConfig, int32 InHighScore, int32 InOrdinal, bool InUnlocked)
{
	ULevelStatus* LevelStatus = NewObject<ULevelStatus>();
	LevelStatus->LevelConfig = InLevelConfig;
	LevelStatus->HighScore = InHighScore;
	LevelStatus->Ordinal = InOrdinal;
	LevelStatus->Unlocked = InUnlocked;

	return LevelStatus;
}

ULevelGroupStatus* ULevelGroupStatus::MakeLevelGroupStatus(FLevelGroup InLevelGroup, TArray<ULevelStatus*> InLevelStatuses)
{
	ULevelGroupStatus* GroupStatus = NewObject<ULevelGroupStatus>();
	GroupStatus->LevelGroup = InLevelGroup;
	GroupStatus->LevelStatuses = InLevelStatuses;

	return GroupStatus;
}

void UVTGameInstance::LoadLevel(ULevelGroupStatus* LevelGroupStatus, ULevelStatus* LevelStatus)
{
	if(!IsValid(LevelStatus))
	{
		UE_LOG(LogTemp, Log, TEXT("Invalid level specified :("));
		return;
	}

	CurrentLevelConfig = LevelStatus->LevelConfig;

	UE_LOG(LogTemp, Log, TEXT("%s - Loading %s"), ANSI_TO_TCHAR(__FUNCTION__), *CurrentLevelConfig.Name);

	FString Map = LevelStatus->LevelConfig.Map;
	if(Map == "")
	{
		Map = LevelGroupStatus->LevelGroup.Map;
	}
	if(Map == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("%s No map specified, using default"), ANSI_TO_TCHAR(__FUNCTION__));
		Map = "DemoMap";
	}

	UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not acquire world instance from this context"));
		return;
	}

	ETravelType TravelType = TRAVEL_Absolute;
	FWorldContext &WorldContext = GEngine->GetWorldContextFromWorldChecked(World);

	GEngine->SetClientTravel(World, *Map, TravelType);
}
