#pragma once

#include "VTDevice.h"
#include "Engine/DataTable.h"
#include "LevelConfig.h"
#include "VTSaveGame.h"
#include "SeafileClient.h"

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VTGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVTDeviceConnectionChanged);

UCLASS(Config=VTSettings)
class VTLEARN_API UVTGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	virtual void Shutdown() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* LevelsDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ULevelGroupStatus*> LevelGroups;

	UPROPERTY(BlueprintReadWrite)
	UVTSaveGame* LoadedSave = nullptr;

	UPROPERTY(BlueprintReadWrite)
	ULevelStatus* CurrentLevelStatus = nullptr;

	UFUNCTION(BlueprintCallable)
	void LoadLevel(ULevelGroupStatus* LevelGroupStatus, ULevelStatus* LevelStatus);

	UFUNCTION(BlueprintCallable)
	UVTSaveGame* AddUser(int32 PID, FString Username);

	UFUNCTION(BlueprintCallable)
	UVTSaveGame* LoadProgress(FString SlotName);

	UFUNCTION(BlueprintPure)
	int32 GetStarCount(UVTSaveGame* SaveGame);

	UPROPERTY(Config, EditAnywhere)
	FString SeafileServer;

	UPROPERTY(Config, EditAnywhere)
	FString SeafileUsername;

	UPROPERTY(Config, EditAnywhere)
	FString SeafilePassword;

	UPROPERTY(Config, EditAnywhere)
	FString SeafileRemotePath;

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

	UFUNCTION(BlueprintPure, meta = (WorldContext = WorldContextObject))
	static UVTGameInstance* GetVTGameInstance(UObject* WorldContextObject);

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

	#pragma endregion
};
