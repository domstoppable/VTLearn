// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PhonePhraseMatcher.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VibeyItemReceiver.generated.h"

UCLASS()
class VTLEARN_API AVibeyItemReceiver : public AActor
{
	GENERATED_BODY()

public:
	AVibeyItemReceiver();

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(BlueprintReadOnly)
	FString HelpText;

	UPROPERTY(BlueprintReadWrite)
	UPhoneSequenceMatcher* Matcher;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetMatchPhrase(UPhoneticPhrase* Phrase);

	UFUNCTION(BlueprintCallable)
	void ReceiveItem(AVibeyItem* Item);

	UFUNCTION(BlueprintImplementableEvent)
	void ItemReceived(AVibeyItem* Item);

	UFUNCTION(BlueprintCallable)
	void SetHelpText(FString Text);

	UFUNCTION(BlueprintImplementableEvent)
	void HelpTextChanged(const FString &Text);
};
