// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "LevelConfig.h"

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "PhonemeTrainingTracker.generated.h"

/**
 *
 */
UCLASS()
class VTLEARN_API UPhonemeTrainingTracker : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, float> TargetPhonemeFrequencies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, float> CurrentPhonemeFrequencies;

	UFUNCTION()
	void SetTargetFrequenciesFromCounts(UDataTable* TargetPhonemeCounts);

	UFUNCTION()
	void UpdateCurrentPhonemeFrequencies(TMap<FString, int32> PhoneCounts);

	UFUNCTION()
	float GetMultiplier(FLevelConfig LevelConfig);
};
