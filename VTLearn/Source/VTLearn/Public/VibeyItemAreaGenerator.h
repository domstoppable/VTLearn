#pragma once

#include "PhoneticPhrase.h"
#include "VibeyItemGenerator.h"
#include "VibeyItem.h"

#include "Components/BoxComponent.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VibeyItemAreaGenerator.generated.h"

UCLASS()
class VTLEARN_API AVibeyItemAreaGenerator : public AVibeyItemGenerator
{
	GENERATED_BODY()

public:
	AVibeyItemAreaGenerator();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AVibeyItem> ItemClass = AVibeyItem::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinTriggerTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxTriggerTime = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* AreaBox;

	UPROPERTY(BlueprintReadWrite)
	AVibeyItem* CurrentItem = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FTimerHandle SpawnDelayTimerHandle;

	UFUNCTION()
	void AreaBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult );

	UFUNCTION()
	void AreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );

	UFUNCTION(BlueprintCallable)
	void SpawnItem();


protected:
	bool IsPlayerInteractionComponent(AActor* Actor, UPrimitiveComponent* Component);

};
