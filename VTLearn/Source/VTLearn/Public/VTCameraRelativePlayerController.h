// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "VTPlayerController.h"

#include "CoreMinimal.h"
#include "VTCameraRelativePlayerController.generated.h"

UCLASS()
class VTLEARN_API AVTCameraRelativePlayerController : public AVTPlayerController
{
	GENERATED_BODY()

public:
	virtual FVector GetForward2D() override;
};
