#pragma once

#include "VTPlayerController.h"
#include "VibeyItem.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VTPlayerSearchController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVTVibeyItemInReach, AVibeyItem*, Item);

/**
 *
 */
UCLASS()
class VTLEARN_API AVTPlayerSearchController : public AVTPlayerController
{
	GENERATED_BODY()


public:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(BlueprintreadWrite)
	AVibeyItem* ReachableItem;

	UPROPERTY(BlueprintAssignable)
	FVTVibeyItemInReach	ItemInReach;

	bool CanRevibe() override;
	void OnRevibe() override;

	virtual bool HoldItem(AActor* Item) override;

};
