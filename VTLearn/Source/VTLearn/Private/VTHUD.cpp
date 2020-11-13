// Fill out your copyright notice in the Description page of Project Settings.


#include "VTHUD.h"

#include "VTPlayerController.h"

AVTHUD::AVTHUD()
{
	StatsWidgetClass = UVTHUDWidget::StaticClass();
	PauseWidgetClass = UVTPauseMenuWidget::StaticClass();
}

void AVTHUD::BeginPlay()
{
	StatsWidget = CreateWidget<UVTHUDWidget>(GetWorld(), StatsWidgetClass);
	PauseWidget = CreateWidget<UVTPauseMenuWidget>(GetWorld(), PauseWidgetClass);

	if(IsValid(StatsWidget))
	{
		StatsWidget->AddToViewport();
	}
}

void AVTHUD::ShowPause()
{
	UE_LOG(LogTemp, Log, TEXT("AVTHUD ShowPause"));

	/*
	if(IsValid(StatsWidget) && StatsWidget->GetParent() != nullptr)
	{
		StatsWidget->RemoveFromParent();
	}
	*/

	if(IsValid(PauseWidget))
	{
		PauseWidget->AddToViewport();
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Pause widget bad"));
	}
}

void AVTHUD::HidePause()
{
	if(IsValid(PauseWidget))
	{
		PauseWidget->RemoveFromParent();
	}

}