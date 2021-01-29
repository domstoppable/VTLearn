// Fill out your copyright notice in the Description page of Project Settings.


#include "VTGameInstance.h"
#include "VTDevice.h"
#include "VTTCPDevice.h"
#include "VTSerialDevice.h"
#include "VTPlayerController.h"
#include "VTLevelProgress.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"


void UVTGameInstance::Init()
{
	if(!LevelsDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Levels Data Table is not configured in VTGameInstance!"));
		return;
	}

	FString ContextString;
	TArray<FName> GroupRowNames = LevelsDataTable->GetRowNames();
	for(FName RowName : GroupRowNames)
	{
		FLevelGroup* LevelGroup = LevelsDataTable->FindRow<FLevelGroup>(RowName, ContextString);
		if(LevelGroup)
		{
			TArray<ULevelStatus*> LevelStatuses;
			if(!IsValid(LevelGroup->LevelConfigs))
			{
				UE_LOG(LogTemp, Warning, TEXT("%s group row has no level data table"), *LevelGroup->Name);
			}
			else
			{
				int Ordinal = 0;
				TArray<FName> LevelRowNames = LevelGroup->LevelConfigs->GetRowNames();
				for(FName LevelRowName : LevelRowNames)
				{
					FLevelConfig* LevelConfig = LevelGroup->LevelConfigs->FindRow<FLevelConfig>(LevelRowName, ContextString);

					ULevelStatus* LevelStatus = NewObject<ULevelStatus>();
					LevelStatus->LevelConfig = *LevelConfig;
					LevelStatus->GroupName = LevelGroup->Name;
					LevelStatus->Ordinal = Ordinal++;
					LevelStatuses.Emplace(LevelStatus);
				}
			}

			ULevelGroupStatus* GroupStatus = NewObject<ULevelGroupStatus>();
			GroupStatus->LevelGroup = *LevelGroup;
			GroupStatus->LevelStatuses = LevelStatuses;
			LevelGroups.Emplace(GroupStatus);
		}
	}
}

void UVTGameInstance::ConnectToTCPDevice(FString IP, int32 Port)
{
	FVTDeviceConnectionChangedDelegate ConnectDelegate;
	ConnectDelegate.BindDynamic(this, &UVTGameInstance::OnDeviceConnected);

	FVTDeviceConnectionChangedDelegate DisconnectDelegate;
	DisconnectDelegate.BindDynamic(this, &UVTGameInstance::OnDeviceDisconnected);

	VTDevice = UVTTCPDevice::ConnectToTCPDevice(IP, Port, ConnectDelegate, DisconnectDelegate);
	VTDevice->WorldContextObject = this;
}

void UVTGameInstance::ConnectToSerialDevice(FString Port, int32 Baud)
{
	FVTDeviceConnectionChangedDelegate ConnectDelegate;
	ConnectDelegate.BindDynamic(this, &UVTGameInstance::OnDeviceConnected);

	FVTDeviceConnectionChangedDelegate DisconnectDelegate;
	DisconnectDelegate.BindDynamic(this, &UVTGameInstance::OnDeviceDisconnected);

	VTDevice = UVTSerialDevice::ConnectToSerialDevice(Port, Baud, ConnectDelegate, DisconnectDelegate);
	VTDevice->WorldContextObject = this;

	// Serial device connects immediately unless there's an error
	if(VTDevice->ConnectionState != EDeviceConnectionState::Connected)
	{
		VTDevice = nullptr;
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

void UVTGameInstance::LoadLevel(ULevelGroupStatus* LevelGroupStatus, ULevelStatus* LevelStatus)
{
	if(!IsValid(LevelStatus))
	{
		UE_LOG(LogTemp, Log, TEXT("Invalid level specified :("));
		return;
	}

	CurrentLevelStatus = LevelStatus;

	UE_LOG(LogTemp, Log, TEXT("%s - Loading %s"), ANSI_TO_TCHAR(__FUNCTION__), *LevelStatus->LevelConfig.Name);

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
	GEngine->SetClientTravel(World, *Map, TravelType);
}

UVTSaveGame* UVTGameInstance::AddUser(int32 PID, FString Username)
{
	USaveGame* Loaded = UGameplayStatics::CreateSaveGameObject(UVTSaveGame::StaticClass());
	LoadedSave = Cast<UVTSaveGame>(Loaded);
	if(LoadedSave)
	{
		LoadedSave->PID = PID;
		LoadedSave->Username = Username;
		SaveProgress();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create new save game"));
	}

	return LoadedSave;
}

UVTSaveGame* UVTGameInstance::LoadProgress(FString SlotName)
{
	UE_LOG(LogTemp, Log, TEXT("Loading save %s"), *SlotName);

	USaveGame* Loaded = UGameplayStatics::LoadGameFromSlot(SlotName, 0);
	LoadedSave = Cast<UVTSaveGame>(Loaded);

	for(ULevelGroupStatus* GroupStatus : LevelGroups)
	{
		bool LockedFromHere = false;
		for(ULevelStatus* LevelStatus : GroupStatus->LevelStatuses)
		{
			LevelStatus->Unlocked = !LockedFromHere;

			FVTLevelProgress Progress = LoadedSave->GetLevelProgress(LevelStatus->GroupName, LevelStatus->LevelConfig.Name);
			LevelStatus->HighScore = Progress.HighScore;

			if(!LockedFromHere)
			{
				if(LevelStatus->LevelConfig.StarThresholds.Num() > 0)
				{
					LockedFromHere = LevelStatus->GetStarCount() < 1;
				}
			}
		}
	}

	return LoadedSave;
}

bool UVTGameInstance::SaveProgress()
{
	UE_LOG(LogTemp, Log, TEXT("Saving progress..."));
	if(IsValid(CurrentLevelStatus))
	{
		if(AVTPlayerController* PlayerController = Cast<AVTPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
		{
			AVTPlayerState* State = PlayerController->GetVTPlayerState();
			LoadedSave->SetHighScore(
				CurrentLevelStatus->GroupName,
				CurrentLevelStatus->LevelConfig.Name,
				State->GetScore()
			);
		}
	}

	bool SaveOK = UGameplayStatics::SaveGameToSlot(LoadedSave, LoadedSave->GetSlotName(), 0);
	if(!SaveOK)
	{
		UE_LOG(LogTemp, Warning, TEXT("Saving failed!"));
	}

	LoadProgress(LoadedSave->GetSlotName());
	return SaveOK;
}

UVTGameInstance* UVTGameInstance::GetVTGameInstance(UObject* WorldContextObject)
{
	return Cast<UVTGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

int32 UVTGameInstance::GetStarCount(UVTSaveGame* SaveGame)
{
	int32 Stars = 0;
	for(ULevelGroupStatus* GroupStatus : LevelGroups)
	{
		bool LockedFromHere = false;
		for(ULevelStatus* LevelStatus : GroupStatus->LevelStatuses)
		{
			Stars += LevelStatus->GetStarCount(SaveGame->GetLevelProgress(LevelStatus->GroupName, LevelStatus->LevelConfig.Name).HighScore);
		}
	}

	return Stars;
}
