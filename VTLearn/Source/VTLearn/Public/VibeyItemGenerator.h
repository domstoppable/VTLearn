// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "PhoneticPhrase.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VibeyItemGenerator.generated.h"

UCLASS()
class VTLEARN_API AVibeyItemGenerator : public AActor
{
	GENERATED_BODY()

public:
	AVibeyItemGenerator();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite)
	TArray<UPhoneticPhrase*> Phrases;

	UPROPERTY(BlueprintReadWrite)
	TArray<UPhoneticPhrase*> CurrentPool;

	UFUNCTION(BlueprintCallable)
	UPhoneticPhrase* RandomPhrase();

	UFUNCTION(BlueprintCallable)
	void AddToPhraseBank(TArray<UPhoneticPhrase*> MorePhrases);
};
