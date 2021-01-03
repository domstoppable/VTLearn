// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "VTPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVTScoreChanged, int32, Delta, int32, Total);

UCLASS(BlueprintType)
class VTLEARN_API AVTPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UPhoneticPhrase*, int32> CorrectCounts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<UPhoneticPhrase*, int32> IncorrectCounts;

	UFUNCTION(BlueprintCallable)
	int32 OnItemAttempted(UPhoneticPhrase* Phrase, bool bCorrect);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetTotalCorrectCount();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetTotalIncorrectCount();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCorrectCountByPhrase(UPhoneticPhrase* Phrase);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetIncorrectCountByPhrase(UPhoneticPhrase* Phrase);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCorrectCountByText(FString WrittenText);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetIncorrectCountByText(FString WrittenText);

	UPROPERTY(BlueprintAssignable)
	FVTScoreChanged ScoreChanged;

protected:
	int32 GetTotal(const TMap<UPhoneticPhrase*, int32>& Map);
	int32 GetCounts(FString WrittenText, const TMap<UPhoneticPhrase*, int32>& Map);
};
