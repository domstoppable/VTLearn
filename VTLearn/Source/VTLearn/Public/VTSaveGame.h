// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "VTLevelProgress.h"
#include "LevelConfig.h"

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "VTSaveGame.generated.h"

UCLASS()
class VTLEARN_API UVTSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVTLevelProgress> Progress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, int32> PhoneCounts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalScore;

	UFUNCTION()
	void AddToTotalScore(int32 Points);

	UFUNCTION(BlueprintPure)
	FString GetSlotName();

	UFUNCTION(BlueprintPure)
	FVTLevelProgress GetLevelProgress(FString GroupName, FString LevelName);

	UFUNCTION(BlueprintPure)
	int32 GetLevelStarCount(ULevelStatus* LevelStatus);

	UFUNCTION(BlueprintPure)
	int32 GetGroupStarCount(ULevelGroupStatus* GroupStatus);

	UFUNCTION(BlueprintCallable)
	void SetHighScore(FString GroupName, FString LevelName, int32 Score);

	UFUNCTION(BlueprintPure, Category = Game)
	static TArray<FString> GetAllSaveGameSlotNames();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = WorldContextObject))
	static TArray<UVTSaveGame*> LoadVTSaveGames(UObject * WorldContextObject);

};
