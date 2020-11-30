// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VTLevelProgress.h"

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

	UFUNCTION(BlueprintPure)
	FString GetSlotName();

	UFUNCTION(BlueprintPure)
	FVTLevelProgress GetLevelProgress(FString GroupName, FString LevelName);

	UFUNCTION(BlueprintCallable)
	void SetHighScore(FString GroupName, FString LevelName, int32 Score);

	UFUNCTION(BlueprintPure, Category = Game)
	static TArray<FString> GetAllSaveGameSlotNames();

};
