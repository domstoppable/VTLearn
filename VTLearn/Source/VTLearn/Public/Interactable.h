// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "CoreMinimal.h"
#include "Interactable.generated.h"

UINTERFACE()
class VTLEARN_API UInteractable : public UInterface
{
	GENERATED_BODY()
};

class VTLEARN_API IInteractable {
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnInteract(const UObject* Instigator);

};
