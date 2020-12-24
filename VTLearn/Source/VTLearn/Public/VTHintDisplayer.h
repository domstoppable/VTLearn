#pragma once

#include "CoreMinimal.h"
#include "VTHintDisplayer.generated.h"

UINTERFACE()
class VTLEARN_API UVTHintDisplayer : public UInterface
{
	GENERATED_BODY()
};

class VTLEARN_API IVTHintDisplayer
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ShowHint(const FString &Text);
};
