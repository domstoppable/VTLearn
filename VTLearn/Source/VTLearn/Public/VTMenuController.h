// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VTMenuController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVTMenuAction);

UCLASS(BlueprintType)
class VTLEARN_API AVTMenuController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FVTMenuAction MenuPrimaryAction;

	UPROPERTY(BlueprintAssignable)
	FVTMenuAction MenuSecondaryAction;

	UPROPERTY(BlueprintAssignable)
	FVTMenuAction MenuTertiaryAction;

	UPROPERTY(BlueprintAssignable)
	FVTMenuAction MenuReject;

	virtual void SetupInputComponent() override;
	virtual void BindInputActions();

	void OnMenuPrimaryAction();
	void OnMenuSecondaryAction();
	void OnMenuTertiaryAction();
	void OnMenuReject();
};
