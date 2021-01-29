// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Misc/Paths.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhoneticPhrase.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EPhoneme : uint8
{
	B,
	D,
	G,
	P,
	T,
	K,
	DX,
	Q,
	JH,
	CH,
	S,
	SH,
	Z,
	ZH,
	F,
	TH,
	V,
	DH,
	M,
	N,
	NG,
	EM,
	EN,
	ENG,
	NX,
	L,
	R,
	W,
	Y,
	HH,
	HV,
	EL,
	IY,
	IH,
	EH,
	EY,
	AE,
	AA,
	AW,
	AY,
	AH,
	AO,
	OY,
	OW,
	UH,
	UW,
	UX,
	ER,
	AX,
	IX,
	AXR,
	AX_H,
	_,
	SP,
	SI
};

USTRUCT(BlueprintType)
struct FVTTSample
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	EPhoneme Phone;

	UPROPERTY(BlueprintReadOnly)
	uint8 Pitch;

	UPROPERTY(BlueprintReadOnly)
	uint8 Intensity;
};

UCLASS(BlueprintType)
class VTLEARN_API UPhoneticPhrase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString SourceFull;

	UPROPERTY(BlueprintReadOnly)
	FString SourceShort;

	UPROPERTY(BlueprintReadOnly)
	FString WrittenText;

	UPROPERTY(BlueprintReadOnly)
	FString PhoneticText;

	UPROPERTY(BlueprintReadOnly)
	TArray<EPhoneme> Phonemes = TArray<EPhoneme>();

	// In MS
	UPROPERTY(BlueprintReadOnly)
	int32 Period;

	UPROPERTY(BlueprintReadOnly)
	TArray<uint8> RawSamples;

	UFUNCTION(BlueprintCallable, Category = "VTT")
	static UPhoneticPhrase* LoadPhoneticPhrase(FString VTTFilename);

	UFUNCTION(BlueprintCallable, Category = "VTT")
	static TArray<UPhoneticPhrase*> LoadPhrasesFromDirectory(FString Path);

	UFUNCTION(BlueprintCallable, Category = "VTT")
	static TArray<UPhoneticPhrase*> LoadPhrases(FString PhraseName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VTT")
	static TArray<EPhoneme> StringToSequence(FString PhoneText);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VTT")
	int32 GetDurationInMS()
	{
		return Period*(RawSamples.Num()/3);
	}

	bool operator==(const UObject* Other) const
	{
		UE_LOG(LogTemp, Log, TEXT("Equality check"));
		if(Other->IsA(UPhoneticPhrase::StaticClass()))
		{
			return ((UPhoneticPhrase*)(Other))->PhoneticText == this->PhoneticText;
		}else{
			return false;
		}
	}
};
