// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "PhoneSequenceMatcher.h"
#include "Interactable.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "VibeyItemReceiver.generated.h"

UCLASS()
class VTLEARN_API AVibeyItemReceiver : public AActor, public IInteractable
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
	void SetMatchPhrases(TArray<UPhoneticPhrase*> Phrases);

	UFUNCTION(BlueprintCallable)
	void ReceiveItem(AVibeyItem* Item);

	UFUNCTION(BlueprintImplementableEvent)
	void ItemReceived(AVibeyItem* Item);

	UFUNCTION(BlueprintCallable)
	void SetHelpText(FString Text);

	UFUNCTION(BlueprintImplementableEvent)
	void HelpTextChanged(const FString &Text);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool AllowAutoAssign = true;
};
