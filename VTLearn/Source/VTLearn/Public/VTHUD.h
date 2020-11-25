// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/WidgetComponent.h"

#include "VTHUDWidget.h"
#include "2D/MenuTree.h"
#include "2D/MenuTreeWidget.h"

#include "VTHUD.generated.h"

/**
 *
 */
UCLASS()
class VTLEARN_API AVTHUD : public AHUD
{
	GENERATED_BODY()

public:
	AVTHUD();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UVTHUDWidget> StatsWidgetClass;

	UPROPERTY(BlueprintReadWrite)
	UVTHUDWidget* StatsWidget;

	UPROPERTY(BlueprintReadWrite)
	UMenuTreeWidget* PauseWidget;

	UPROPERTY(EditDefaultsOnly)
	UMenuTree* PauseMenuTree;

	UFUNCTION(BlueprintCallable)
	void ShowPause();

	UFUNCTION(BlueprintCallable)
	void HidePause();
};
