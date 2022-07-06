// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VTBlueprintLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FDownloadFinished, const TArray<FString>&, UserIDs, const TArray<int32>&, Scores);

UCLASS()
class VTLEARN_API UVTBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"), Category="VTT")
	static void DownloadHighScores(const UObject* WorldContextObject, const FDownloadFinished& DownloadFinished);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VTT")
	static FString GetBuildDate();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VTT")
	static FString GetBuildTime();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VTT")
	static FString GetBuildString();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VTT")
	static FString GetProjectSetting(FString Name);
};
