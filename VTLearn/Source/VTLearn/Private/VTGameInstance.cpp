// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VTGameInstance.h"
#include "VTDevice.h"
#include "VTTCPDevice.h"
#include "VTSerialDevice.h"
#include "VTPlayerController.h"
#include "VTLevelProgress.h"
#include "SeafileClient.h"

#include "Kismet/GameplayStatics.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Misc/Guid.h"

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

	UploadTrainingLogs();
}

void UVTGameInstance::ConnectToTCPDevice(FString IP, int32 Port)
{
	FVTDeviceConnectionChangedDelegate ConnectDelegate;
	ConnectDelegate.BindDynamic(this, &UVTGameInstance::OnDeviceConnected);

	FVTDeviceConnectionChangedDelegate DisconnectDelegate;
	DisconnectDelegate.BindDynamic(this, &UVTGameInstance::OnDeviceDisconnected);

	VTDevice = UVTTCPDevice::ConnectToTCPDevice(IP, Port, ConnectDelegate, DisconnectDelegate, this);
}

void UVTGameInstance::ConnectToSerialDevice(FString Port, int32 Baud)
{
	UE_LOG(LogTemp, Log, TEXT("UVTGameInstance::ConnectToSerialDevice"));

	FVTDeviceConnectionChangedDelegate ConnectDelegate;
	ConnectDelegate.BindDynamic(this, &UVTGameInstance::OnDeviceConnected);

	FVTDeviceConnectionChangedDelegate DisconnectDelegate;
	DisconnectDelegate.BindDynamic(this, &UVTGameInstance::OnDeviceDisconnected);

	VTDevice = UVTSerialDevice::ConnectToSerialDevice(Port, Baud, ConnectDelegate, DisconnectDelegate, this);

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

	CurrentGroupStatus = LevelGroupStatus;
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

	LevelAttemptGuid = FGuid::NewGuid().ToString();
	if(VTDevice && IsValid(VTDevice))
	{
		StartNewStimulusLog();
	}
	ETravelType TravelType = TRAVEL_Absolute;
	GEngine->SetClientTravel(World, *Map, TravelType);

	// Garbage collector is going to sweep this. Null it out now so it can be recreated later
	PhonemeTrainingTracker = nullptr;
}

void UVTGameInstance::ReloadLevel()
{
	LoadLevel(CurrentGroupStatus, CurrentLevelStatus);
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

	GetPhonemeTrainingTracker();
	PhonemeTrainingTracker->UpdateCurrentPhonemeFrequencies(LoadedSave->PhoneCounts);

	float MaxMultiplier = 1.0f;

	for(ULevelGroupStatus* GroupStatus : LevelGroups)
	{
		bool LockedFromHere = false;
		for(ULevelStatus* LevelStatus : GroupStatus->LevelStatuses)
		{
			LevelStatus->Unlocked = !LockedFromHere;

			FVTLevelProgress Progress = LoadedSave->GetLevelProgress(LevelStatus->GroupName, LevelStatus->LevelConfig.Name);
			LevelStatus->HighScore = Progress.HighScore;
			LevelStatus->Multiplier = PhonemeTrainingTracker->GetMultiplier(LevelStatus->LevelConfig);
			if(LevelStatus->Multiplier > MaxMultiplier)
			{
				MaxMultiplier = LevelStatus->Multiplier;
			}

			if(!LockedFromHere && !bUnlockAllLevels)
			{
				if(LevelStatus->LevelConfig.StarThresholds.Num() > 0)
				{
					LockedFromHere = LevelStatus->GetStarCount() < 1;
				}
			}
		}
	}

	for(ULevelGroupStatus* GroupStatus : LevelGroups)
	{
		for(ULevelStatus* LevelStatus : GroupStatus->LevelStatuses)
		{
			LevelStatus->Multiplier = 1.0f + LevelStatus->Multiplier / MaxMultiplier * 2;
			if(LevelStatus->Multiplier < 1.0f)
			{
				LevelStatus->Multiplier = 1.0f;
			}
		}
	}

	if(LoadedSave->PhoneCounts.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("No phones trained yet"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Phone counts:"));
		LoadedSave->PhoneCounts.KeySort([](FString A, FString B) {
			return A < B;
		});
		for (auto& Elem : LoadedSave->PhoneCounts)
		{
			UE_LOG(LogTemp, Log, TEXT("\t%s\t: %d"), *Elem.Key, Elem.Value);
		}
	}

	return LoadedSave;
}

UPhonemeTrainingTracker* UVTGameInstance::GetPhonemeTrainingTracker()
{
	if(!IsValid(PhonemeTrainingTracker))
	{
		PhonemeTrainingTracker = NewObject<UPhonemeTrainingTracker>();
		PhonemeTrainingTracker->SetTargetFrequenciesFromCounts(TargetPhonemeCounts);
	}
	return PhonemeTrainingTracker;
}

bool UVTGameInstance::SaveProgress()
{
	UploadTrainingLogs();

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
			LoadedSave->AddToTotalScore(State->GetScore() * CurrentLevelStatus->Multiplier);
		}
	}
	UploadHighScore();

	bool SaveOK = UGameplayStatics::SaveGameToSlot(LoadedSave, LoadedSave->GetSlotName(), 0);
	if(!SaveOK)
	{
		UE_LOG(LogTemp, Warning, TEXT("Saving failed!"));
	}

	LoadProgress(LoadedSave->GetSlotName());
	return SaveOK;
}

UVTGameInstance* UVTGameInstance::GetVTGameInstance(const UObject* WorldContextObject)
{
	return Cast<UVTGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

bool UVTGameInstance::WorldIsTearingDown(UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->bIsTearingDown;
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

void UVTGameInstance::UploadTrainingLogs()
{
	if(SeafileServer.Equals(""))
	{
		UE_LOG(LogTemp, Error, TEXT("Seafile server not configured! Training logs will NOT be uploaded!"));
		return;
	}

	FilesToUpload.Empty();
	FString SearchPath = *(FPaths::ProjectLogDir() + "TrainingData/*");
	IFileManager::Get().FindFiles(FilesToUpload, *SearchPath, true, false);

	if(FilesToUpload.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("No files to upload in '%s'"), *SearchPath);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("%d files are ready to upload"), FilesToUpload.Num());

	FHTTPActionComplete AuthDelegate;
	AuthDelegate.BindDynamic(this, &UVTGameInstance::OnSeafileAuthComplete);

	SeafileClient = NewObject<USeafileClient>();
	SeafileClient->Authenticate(SeafileServer, SeafileUsername, SeafilePassword, AuthDelegate);
}

void UVTGameInstance::OnSeafileAuthComplete(bool Success)
{
	FString BasePath = *(FPaths::ProjectLogDir() + "TrainingData/");

	for(FString File : FilesToUpload)
	{
		FUploadComplete UploadComplete;
		UploadComplete.BindDynamic(this, &UVTGameInstance::OnUploadComplete);

		SeafileClient->UploadFile(BasePath + File, SeafileRemotePath, SeafileRepoID, "", UploadComplete);
	}
}

void UVTGameInstance::OnUploadComplete(bool Success, FString Filename)
{
	if(Success)
	{
		FString UploadedPath = FPaths::GetPath(Filename) + "/Uploaded/";
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*UploadedPath);

		FString Destination = (UploadedPath + FPaths::GetCleanFilename(Filename));
		IFileManager::Get().Move(*Destination, *Filename);
	}
}

void UVTGameInstance::StartNewStimulusLog()
{
	FString Filename = FString::Printf(TEXT("StimulusLog-%04d-%s"), LoadedSave->PID, *LevelAttemptGuid);

	StimulusLogger = UPsydekickData::CreateCSVLogger(Filename, TEXT("TrainingData"));

	TArray<FString> FieldNames;
	FieldNames.Add(TEXT("PID"));
	FieldNames.Add(TEXT("Level"));
	FieldNames.Add(TEXT("LevelAttemptGuid"));
	FieldNames.Add(TEXT("Stimulus"));
	FieldNames.Add(TEXT("Filename"));
	FieldNames.Add(TEXT("Ok"));

	StimulusLogger->SetFieldNames(FieldNames);
}

void UVTGameInstance::LogStimulus(UPhoneticPhrase* Phrase, bool bSendOk)
{
	if(!IsValid(StimulusLogger))
	{
		StartNewStimulusLog();
	}

	TMap<FString, FString> LogRecord;

	LogRecord.Add("PID", FString::Printf(TEXT("%d"), LoadedSave->PID));
	LogRecord.Add("Level", CurrentLevelStatus->LevelConfig.Name);
	LogRecord.Add("LevelAttemptGuid", LevelAttemptGuid);
	LogRecord.Add("Stimulus", FPaths::GetCleanFilename(Phrase->Source));
	LogRecord.Add("Filename", FPaths::GetCleanFilename(StimulusLogger->Filename));
	LogRecord.Add("Ok", bSendOk ? TEXT("1") : TEXT("0"));

	StimulusLogger->LogStrings(LogRecord);

	if(bSendOk)
	{
		for (auto& Elem : Phrase->PhoneCounts)
		{
			int32 Value = LoadedSave->PhoneCounts.FindOrAdd(Elem.Key, 0);
			LoadedSave->PhoneCounts.Add(Elem.Key, Value + Elem.Value);
		}
	}
}

void UVTGameInstance::UploadHighScore()
{
	if(LeaderboardServer.Equals(""))
	{
		UE_LOG(LogTemp, Error, TEXT("Leaderboard server not configured!"));
		return;
	}

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	FString Url = FString::Printf(
		TEXT("%s/api/score/%d/%d/%d"),
		*LeaderboardServer,
		LoadedSave->PID,
		LoadedSave->TotalScore,
		LoadedSave->TotalScore % (LoadedSave->PID+7)
	);
	Request->SetURL(Url);
	Request->SetVerb("POST");

	UE_LOG(LogTemp, Log, TEXT("Uploading high score %d..."), LoadedSave->TotalScore);
	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success){
		if(Success && Response.IsValid())
		{
			if (!EHttpResponseCodes::IsOk(Response->GetResponseCode()))
			{
				UE_LOG(LogTemp, Error, TEXT("Failed up upload scores! [%d]"), Response->GetResponseCode());
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Score upload ok!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to upload score!"));
		}
	});

	Request->ProcessRequest();
}
