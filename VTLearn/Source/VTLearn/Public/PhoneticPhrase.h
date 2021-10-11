// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "Misc/Paths.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhoneticPhrase.generated.h"

USTRUCT(BlueprintType)
struct FPhoneCount : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	FString TrainPhoneme;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="")
	int32 PhonemeExposures;
};

UCLASS(BlueprintType)
class VTLEARN_API UPhoneticPhrase : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString WrittenText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString PhoneticText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Source;

	// In MS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Period;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<uint8> RawSamples;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TMap<FString, int32> PhoneCounts;

	static TArray<UPhoneticPhrase*> LoadPhrases(FString PhraseName);

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
