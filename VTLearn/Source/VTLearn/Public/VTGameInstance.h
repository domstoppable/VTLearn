#pragma once

#include "VTDevice.h"
#include "Engine/DataTable.h"
#include "LevelConfig.h"
#include "VTSaveGame.h"
#include "SeafileClient.h"
#include "Data/CSVLogger.h"
#include "PhonemeTrainingTracker.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VTGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVTDeviceConnectionChanged);

UCLASS()
class VTLEARN_API UVTGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	virtual void Shutdown() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* LevelsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* TargetPhonemeCounts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUnlockAllLevels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ULevelGroupStatus*> LevelGroups;

	UPROPERTY(BlueprintReadWrite)
	UVTSaveGame* LoadedSave = nullptr;

	UPROPERTY(BlueprintReadWrite)
	ULevelGroupStatus* CurrentGroupStatus = nullptr;

	UPROPERTY(BlueprintReadWrite)
	ULevelStatus* CurrentLevelStatus = nullptr;

	UPhonemeTrainingTracker* PhonemeTrainingTracker = nullptr;

	UFUNCTION(BlueprintCallable)
	UPhonemeTrainingTracker* GetPhonemeTrainingTracker();

	UPROPERTY(BlueprintReadOnly)
	FString LevelAttemptGuid;

	UFUNCTION(BlueprintCallable)
	void LoadLevel(ULevelGroupStatus* LevelGroupStatus, ULevelStatus* LevelStatus);

	UFUNCTION(BlueprintCallable)
	void ReloadLevel();

	UFUNCTION(BlueprintCallable)
	UVTSaveGame* AddUser(int32 PID, FString Username);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCSVLogger* StimulusLogger = nullptr;

	UFUNCTION(BlueprintCallable)
	void StartNewStimulusLog();

	UFUNCTION(BlueprintCallable)
	void LogStimulus(UPhoneticPhrase* Phrase, bool bSendOk);

	UFUNCTION(BlueprintCallable)
	UVTSaveGame* LoadProgress(FString SlotName);

	UFUNCTION(BlueprintPure)
	int32 GetStarCount(UVTSaveGame* SaveGame);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USeafileClient* SeafileClient;

	TArray<FString> FilesToUpload;

	UFUNCTION(BlueprintCallable)
	void UploadTrainingLogs();

	UFUNCTION()
	void OnSeafileAuthComplete(bool Success);

	UFUNCTION()
	void OnUploadComplete(bool Success, FString Filename);

	UFUNCTION(BlueprintCallable)
	bool SaveProgress();

	UFUNCTION(BlueprintPure, meta = (WorldContext = WorldContextObject, CompactNodeTitle = "VTGameInstance", Category = "VTT"))
	static UVTGameInstance* GetVTGameInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = WorldContextObject, CompactNodeTitle = "Tearing Down", Category = "VTT"))
	static bool WorldIsTearingDown(UObject* WorldContextObject);

	#pragma region Settings

	UPROPERTY(BlueprintReadOnly)
	int32 DefaultPID;

	UPROPERTY(BlueprintReadOnly)
	FString DefaultUsername;

	UPROPERTY(BlueprintReadOnly)
	FString LeaderboardServer;

	UPROPERTY(BlueprintReadOnly)
	FString ContactInfo;

	UPROPERTY(BlueprintReadOnly)
	FString SeafileServer;

	UPROPERTY(BlueprintReadOnly)
	FString SeafileRepoID;

	UPROPERTY(BlueprintReadOnly)
	FString SeafileUsername;

	UPROPERTY(BlueprintReadOnly)
	FString SeafilePassword;

	UPROPERTY(BlueprintReadOnly)
	FString SeafileRemotePath;

	#pragma endregion

	#pragma region Device client

	UPROPERTY(BlueprintReadOnly)
	UVTDevice* VTDevice;

	UPROPERTY(BlueprintAssignable)
	FVTDeviceConnectionChanged DeviceConnected;

	UPROPERTY(BlueprintAssignable)
	FVTDeviceConnectionChanged DeviceDisconnected;

	UFUNCTION(BlueprintCallable)
	void ConnectToTCPDevice(FString IP, int32 Port);

	UFUNCTION(BlueprintCallable)
	void ConnectToSerialDevice(FString Port, int32 Baud);

	UFUNCTION()
	void OnDeviceConnected();

	UFUNCTION()
	void OnDeviceDisconnected();

	UFUNCTION()
	void UploadHighScore();

	#pragma endregion
};
