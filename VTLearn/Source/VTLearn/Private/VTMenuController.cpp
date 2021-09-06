// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VTMenuController.h"

void AVTMenuController::SetupInputComponent()
{
	Super::SetupInputComponent();
	check(InputComponent);

	BindInputActions();
}

void AVTMenuController::BindInputActions()
{
	InputComponent->BindAction("MenuPrimary", IE_Pressed, this, &AVTMenuController::OnMenuPrimaryAction);
	InputComponent->BindAction("MenuSecondary", IE_Pressed, this, &AVTMenuController::OnMenuSecondaryAction);
	InputComponent->BindAction("MenuTertiary", IE_Pressed, this, &AVTMenuController::OnMenuTertiaryAction);
	InputComponent->BindAction("MenuReject", IE_Pressed, this, &AVTMenuController::OnMenuReject);
}

void AVTMenuController::OnMenuPrimaryAction()
{
	MenuPrimaryAction.Broadcast();
}

void AVTMenuController::OnMenuSecondaryAction()
{
	MenuSecondaryAction.Broadcast();
}

void AVTMenuController::OnMenuTertiaryAction()
{
	MenuTertiaryAction.Broadcast();
}

void AVTMenuController::OnMenuReject()
{
	MenuReject.Broadcast();
}

