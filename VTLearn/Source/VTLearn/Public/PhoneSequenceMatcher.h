// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PhoneticPhrase.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhoneSequenceMatcher.generated.h"

UCLASS(BlueprintType)
class VTLEARN_API UPhoneSequenceMatcher : public UObject
{
	GENERATED_BODY()

public:
	TArray<FString> SearchStrings;

	void SetMatchPhrases(TArray<UPhoneticPhrase*> Phrases);

	UFUNCTION(BlueprintCallable)
	bool Match(UPhoneticPhrase* Phrase);
};
