// Fill out your copyright notice in the Description page of Project Settings.


#include "VTHUD.h"

#include "VTPlayerController.h"


void AVTHUD::BeginPlay()
{
	Widget = CreateWidget<UVTHUDWidget>(GetWorld(), WidgetClass);
	if(Widget)
	{
		Widget->AddToViewport();
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Widget failure"));
	}
}
