// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VTHUDWidget.generated.h"

UCLASS()
class VTLEARN_API UVTHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void ScoreChanged(int32 Delta, int32 Total);
};
