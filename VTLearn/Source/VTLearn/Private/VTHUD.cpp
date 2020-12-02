// Fill out your copyright notice in the Description page of Project Settings.


#include "VTHUD.h"

#include "VTPlayerController.h"
#include "2D/MenuTreeWidget.h"
#include "Misc/PKTools.h"

AVTHUD::AVTHUD()
{
	SetTickableWhenPaused(true);

	StatsWidgetClass = UVTHUDWidget::StaticClass();

	FString ClassRefName = "WidgetBlueprint'/Psydekick/Visuals/2D/Menu/Runtime/MenuTreeWidgetBP.MenuTreeWidgetBP'";
	PauseWidgetClass = PKTools::GetBlueprintClass(ClassRefName);
	if(PauseWidgetClass)
	{
		PauseWidget = CreateWidget<UMenuTreeWidget>(GetWorld(), PauseWidgetClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not load widget class %s"), *ClassRefName);
	}
}

void AVTHUD::BeginPlay()
{
	Super::BeginPlay();
	StatsWidget = CreateWidget<UVTHUDWidget>(GetWorld(), StatsWidgetClass);

	if(IsValid(StatsWidget))
	{
		StatsWidget->AddToViewport();
	}

	if(IsValid(PauseWidget))
	{
		PauseWidget->LoadMenuTree(PauseMenuTree);
		PauseWidget->AnimateOutFinished.BindDynamic(this, &AVTHUD::OnPauseAnimOutDone);
	}

	if(IsValid(InstructionsWidgetClass))
	{
		UUserWidget* InstructionsWidget = CreateWidget<UUserWidget>(GetWorld(), InstructionsWidgetClass);
		InstructionsWidget->AddToViewport();
	}

	if(AVTPlayerController* PlayerController = Cast<AVTPlayerController>(PlayerOwner))
	{
		PlayerController->ScoreChanged.AddDynamic(this, &AVTHUD::ScoreChanged);
	}
}

void AVTHUD::ShowPause()
{
	if(IsValid(PauseWidget))
	{
		PauseWidget->AnimateIn();
		PauseWidget->AddToViewport();
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Pause widget bad :("));
	}
}

void AVTHUD::HidePause()
{
	if(IsValid(PauseWidget))
	{
		PauseWidget->AnimateOut();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No pause widget :("));
	}
}

void AVTHUD::OnPauseAnimOutDone()
{
	PauseWidget->RemoveFromParent();
}

void AVTHUD::ScoreChanged(int32 Delta, int32 Total)
{
	if(IsValid(StatsWidget))
	{
		StatsWidget->ScoreChanged(Delta, Total);
	}
}

void AVTHUD::ShowLevelComplete()
{
	if(!IsValid(LevelCompleteWidgetClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid LevelComplete widget class in HUD"));
		return;
	}

	if(IsValid(StatsWidget))
	{
		StatsWidget->RemoveFromParent();
	}

	UUserWidget* LevelCompleteWidget = CreateWidget<UUserWidget>(GetWorld(), LevelCompleteWidgetClass);
	LevelCompleteWidget->AddToViewport();
}
