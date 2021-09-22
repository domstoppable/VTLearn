// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "Data/PsydekickData.h"
#include "Data/CSVLogger.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "VTPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVTScoreChanged, int32, Delta, int32, Total);

USTRUCT(BlueprintType)
struct FPhrasePerformance
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Correct = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Incorrect = 0;
};

UCLASS(BlueprintType)
class VTLEARN_API AVTPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCSVLogger* DataLogger = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UPhoneticPhrase*, FPhrasePerformance> Counts;

	UPROPERTY(BlueprintAssignable)
	FVTScoreChanged ScoreChanged;

	UFUNCTION(BlueprintCallable)
	int32 OnItemAttempted(UPhoneticPhrase* Phrase, bool bCorrect, bool bExpired);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetTotalCorrectCount();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetTotalIncorrectCount();

	UFUNCTION(BlueprintCallable)
	TMap<FString, FPhrasePerformance> ReportByText();

	UFUNCTION(BlueprintCallable)
	void StartNewLogger();

};
