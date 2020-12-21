// Fill out your copyright notice in the Description page of Project Settings.


#include "VTCameraRelativePlayerController.h"
//#include "VTPlayerController.h"


FVector AVTCameraRelativePlayerController::GetForward2D()
{
	APawn* Pawn = GetPawn();
	if(!IsValid(Pawn))
	{
		return Super::GetForward2D();
	}

	FVector PawnPosition = Pawn->GetTransform().GetTranslation();
	FVector CameraPosition = PlayerCameraManager->GetViewTarget()->GetTransform().GetTranslation();

	return (PawnPosition - CameraPosition).GetSafeNormal2D();
}
