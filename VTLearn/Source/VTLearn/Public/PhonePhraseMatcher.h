// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PhoneticPhrase.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhonePhraseMatcher.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class VTLEARN_API UPhonePhraseMatcher : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "VTT")
	virtual bool Match(UPhoneticPhrase* Phrase);

};

UCLASS(BlueprintType)
class VTLEARN_API UPhoneSequenceMatcher : public UPhonePhraseMatcher
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FString SearchString;

	bool Match(UPhoneticPhrase* Phrase) override;
};
