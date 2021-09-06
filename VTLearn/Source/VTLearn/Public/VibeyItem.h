// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"

#include "PhoneticPhrase.h"

#include "VibeyItem.generated.h"

UCLASS(BlueprintType)
class VTLEARN_API AVibeyItem : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AVibeyItem();

	UPROPERTY(BlueprintReadOnly)
	UPhoneticPhrase* Phrase = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bGrabbable = true;

	UPROPERTY(BlueprintReadWrite)
	bool bIsGood = false;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPhrase(UPhoneticPhrase* NewPhrase);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Grabbed(AActor* Grabber);
};
