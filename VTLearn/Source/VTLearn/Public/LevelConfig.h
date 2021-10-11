#pragma once

#include "Engine/DataTable.h"
#include "VTLevelProgress.h"

#include "CoreMinimal.h"
#include "LevelConfig.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	float TimeLimit = 180.0f;
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
};

UCLASS(BlueprintType)
class VTLEARN_API ULevelStatus : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLevelConfig LevelConfig;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString GroupName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Unlocked;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 HighScore;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Ordinal;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Multiplier = 1.0f;

	UFUNCTION(BlueprintPure, meta=(AdvancedDisplay=0))
	int32 GetStarCount(int32 Score=-1);

	UFUNCTION(BlueprintPure, meta=(WorldContext=WorldContextObject))
	FVTLevelProgress GetProgress(const UObject* WorldContextObject);
};