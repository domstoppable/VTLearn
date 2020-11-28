// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VTNetworkClient.h"
#include "Engine/DataTable.h"


#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "VTGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVTDeviceConnectionChanged);

USTRUCT(BlueprintType)
struct FLevelConfig : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	FString Instructions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	TArray<FString> TrainingPhrases;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	TArray<FString> DistractorPhrases;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	TArray<int32> StarThresholds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	FString Map;
};

USTRUCT(BlueprintType)
struct FLevelGroup : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	UDataTable* LevelConfigs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	FString Map;
};

UCLASS(BlueprintType)
class VTLEARN_API ULevelGroupStatus : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLevelGroup LevelGroup;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<ULevelStatus*> LevelStatuses;

	UFUNCTION(BlueprintCallable)
	static ULevelGroupStatus* MakeLevelGroupStatus(FLevelGroup InLevelGroup, TArray<ULevelStatus*> InLevelStatuses);
};

UCLASS(BlueprintType)
class VTLEARN_API ULevelStatus : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLevelConfig LevelConfig;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Unlocked;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 HighScore;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Ordinal;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 StarCount();

	UFUNCTION(BlueprintCallable)
	static ULevelStatus* MakeLevelStatus(FLevelConfig InLevelConfig, int32 InHighScore, int32 InOrdinal, bool InUnlocked);
};

UCLASS()
class VTLEARN_API UVTGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UVTGameInstance();

	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
	void LoadLevel(ULevelGroupStatus* LevelGroupStatus, ULevelStatus* LevelStatus);

	UPROPERTY(BlueprintReadOnly)
	FLevelConfig CurrentLevelConfig;

	#pragma region Device client

	UPROPERTY(BlueprintReadOnly)
	UVTNetworkClient* VTDevice;

	UPROPERTY(BlueprintAssignable)
	FVTDeviceConnectionChanged DeviceConnected;

	UPROPERTY(BlueprintAssignable)
	FVTDeviceConnectionChanged DeviceDisconnected;

	UFUNCTION(BlueprintCallable)
	void ConnectToDevice(FString IP, int32 Port);

	UFUNCTION()
	void OnDeviceConnected();

	UFUNCTION()
	void OnDeviceDisconnected();

	#pragma endregion
};
