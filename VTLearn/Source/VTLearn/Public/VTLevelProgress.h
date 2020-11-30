#pragma once

#include "CoreMinimal.h"
#include "VTLevelProgress.generated.h"

USTRUCT(BlueprintType)
struct FVTLevelProgress
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyCategory)
	int32 HighScore = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyCategory)
	FString GroupName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MyCategory)
	FString LevelName;
};
