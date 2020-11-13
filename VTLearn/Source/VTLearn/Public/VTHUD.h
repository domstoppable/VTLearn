// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/WidgetComponent.h"

#include "VTHUDWidget.h"

#include "VTHUD.generated.h"

/**
 *
 */
UCLASS()
class VTLEARN_API AVTHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UVTHUDWidget> WidgetClass;

	UPROPERTY(BlueprintReadWrite)
	UVTHUDWidget* Widget;
};
