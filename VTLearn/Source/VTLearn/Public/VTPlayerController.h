// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "VTPlayerState.h"
#include "VTMenuController.h"
#include "Interactable.h"

#include "GameFramework/Actor.h"

#include "Sound/SoundWave.h"
#include "Components/AudioComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VTPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVTPauseChanged, bool, Paused);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVTAppFocusChanged, bool, IsFocused);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVTVibeyItemInReach, AVibeyItem*, Item);

UCLASS()
class VTLEARN_API AVTPlayerController : public AVTMenuController
{
	GENERATED_BODY()

public:
	AVTPlayerController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintReadWrite)
	USoundWave* NoiseWave = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UAudioComponent* NoiseAudio = nullptr;

	// @TODO: move this to character class
	UPROPERTY(BlueprintReadOnly)
	AActor* HeldItem;

	UPROPERTY(BlueprintAssignable)
	FVTPauseChanged PauseChanged;

	UPROPERTY(BlueprintAssignable)
	FVTAppFocusChanged AppFocusChanged;

	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> Reachables;

	UPROPERTY(BlueprintAssignable)
	FVTVibeyItemInReach	ItemInReach;

	UPROPERTY(BlueprintReadWrite)
	float DashSpeed = 250000.0f;

	virtual void BindInputActions() override;

	virtual void OnMoveUp(float Value);
	virtual void OnMoveRight(float Value);
	virtual void OnJump();
	virtual void OnGrab();
	virtual void OnRevibe();
	virtual void OnInteract();

	UFUNCTION(BlueprintCallable)
	virtual void OnDash();

	UFUNCTION(BlueprintCallable)
	void TogglePause();

	virtual void Pause() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual FVector GetForward2D();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual bool CanInteract();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual bool CanRevibe();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual bool CanHold(AActor* Item);

	UFUNCTION(BlueprintCallable)
	virtual bool HoldItem(AActor* Item);

	UFUNCTION(BlueprintCallable)
	void DropItem();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AVTPlayerState* GetVTPlayerState();

	UFUNCTION()
	void OnLevelTimedOut();

	UFUNCTION(BlueprintPure, meta = (WorldContext = WorldContextObject))
	static AVTPlayerController* GetVTPlayerController(UObject* WorldContextObject);

	UFUNCTION()
	void OnWindowFocusChanged(bool bIsFocused);

	UFUNCTION()
	virtual void OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	template<class T>
	T* GetFirstReachableOfClass()
	{
		for (AActor* Interactable : Reachables)
		{
			if (T* Object = Cast<T>(Interactable))
			{
				return Object;
			}
		}

		return nullptr;
	}
};
