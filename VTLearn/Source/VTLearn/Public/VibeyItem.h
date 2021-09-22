// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"

#include "PhoneticPhrase.h"
#include "Interactable.h"

#include "VibeyItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVibeyItemAttempted, AVibeyItem*, VibeyItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVibeyItemExpired, AVibeyItem*, VibeyItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVibeyItemPickedUp, AVibeyItem*, VibeyItem);

UCLASS(BlueprintType)
class VTLEARN_API AVibeyItem : public AStaticMeshActor, public IInteractable
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

	UPROPERTY(BlueprintReadWrite)
	bool bIsExpired = false;

	UPROPERTY(BlueprintAssignable)
	FVibeyItemExpired Expired;

	UPROPERTY(BlueprintAssignable)
	FVibeyItemAttempted Attempted;

	UPROPERTY(BlueprintAssignable)
	FVibeyItemPickedUp Grabbed;

	UPROPERTY(BlueprintReadWrite)
	FTimerHandle ExpirationTimerHandle;

	UFUNCTION()
	void StartExpirationTimer(float Period);

protected:
	virtual void BeginPlay() override;

	virtual void ExpireNow();

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPhrase(UPhoneticPhrase* NewPhrase);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnGrabbed(AActor* Grabber);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnExpired();

	UFUNCTION(BlueprintCallable)
	void MarkAttempted(bool bNewIsGood);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnAttempted();
};
