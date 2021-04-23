// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VTLevelProgress.h"
#include "LevelConfig.h"

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "VTSaveGame.generated.h"

UCLASS(Config=VTSettings)
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

	UPROPERTY(Config)
	int32 DefaultPID;

	UPROPERTY(Config)
	FString DefaultUsername;

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

	UFUNCTION(BlueprintCallable)
	static TArray<UVTSaveGame*> LoadVTSaveGames();
};
