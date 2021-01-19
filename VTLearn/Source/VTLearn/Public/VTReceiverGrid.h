#pragma once

#include "VibeyItemReceiver.h"

#include "Components/BoxComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VTReceiverGrid.generated.h"

UCLASS(BlueprintType)
class VTLEARN_API AVTReceiverGrid : public AActor
{
	GENERATED_BODY()

public:
	AVTReceiverGrid();

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* AreaBox;

	UPROPERTY(EditAnywhere)
	int32 XCount = 1;

	UPROPERTY(EditAnywhere)
	int32 YCount = 1;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AVibeyItemReceiver> ReceiverClass;

	UPROPERTY(BlueprintReadWrite)
	TArray<AVibeyItemReceiver*> Receivers;

	TArray<TArray<FVector>> GridWorldLocations;

public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetOpenLocations();

	UFUNCTION(BlueprintCallable)
	void MoveToRandomCell(AVibeyItemReceiver* Receiver);
};
