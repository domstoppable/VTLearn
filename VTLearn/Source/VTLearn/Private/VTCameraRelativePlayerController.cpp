// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VTCameraRelativePlayerController.h"
#include "GameFramework/Pawn.h"

FVector AVTCameraRelativePlayerController::GetForward2D()
{
	APawn* PlayerPawn = GetPawn();
	if(!IsValid(PlayerPawn))
	{
		return Super::GetForward2D();
	}

	FVector PawnPosition = PlayerPawn->GetTransform().GetTranslation();
	FVector CameraPosition = PlayerCameraManager->GetViewTarget()->GetTransform().GetTranslation();

	return (PawnPosition - CameraPosition).GetSafeNormal2D();
}
