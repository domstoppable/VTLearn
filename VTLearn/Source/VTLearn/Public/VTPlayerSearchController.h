// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "VTPlayerController.h"
#include "VibeyItem.h"

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VTPlayerSearchController.generated.h"

/**
 *
 */
UCLASS()
class VTLEARN_API AVTPlayerSearchController : public AVTPlayerController
{
	GENERATED_BODY()


public:
	virtual void BeginPlay() override;

	virtual void OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual bool CanRevibe() override;
	virtual void OnRevibe() override;

	virtual bool HoldItem(AActor* Item) override;

};
