#pragma once

#include "VTHintDisplayer.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VTLearnCharacter.generated.h"

UCLASS()
class VTLEARN_API AVTLearnCharacter : public ACharacter, public IVTHintDisplayer
{
	GENERATED_BODY()

public:
	AVTLearnCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ItemGrabbed(AActor* Item);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ItemDropped(AActor* Item);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Speak(const FString &Text, const float Duration);

};
