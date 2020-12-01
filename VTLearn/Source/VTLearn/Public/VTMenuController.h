// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VTMenuController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVTMenuAction);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVTPauseChanged, bool, Paused);

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

	void OnMenuPrimaryAction();
	void OnMenuSecondaryAction();
	void OnMenuTertiaryAction();
	void OnMenuReject();
};
