#include "VTHUD.h"

#include "VTPlayerController.h"
#include "2D/MenuTreeWidget.h"
#include "Misc/PKTools.h"

AVTHUD::AVTHUD()
{
	SetTickableWhenPaused(true);
	StatsWidgetClass = UVTHUDWidget::StaticClass();
}

void AVTHUD::BeginPlay()
{
	Super::BeginPlay();
	StatsWidget = CreateWidget<UVTHUDWidget>(GetWorld(), StatsWidgetClass);

	if(IsValid(StatsWidget))
	{
		StatsWidget->AddToViewport();
	}

	if(PauseWidgetClass)
	{
		UE_LOG(LogTemp, Log, TEXT("Creating pause widget"));
		PauseWidget = CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass);
	}

	if(IsValid(PauseWidget))
	{
		if(UMenuTreeWidget* PauseMenu = Cast<UMenuTreeWidget>(PauseWidget))
		{
			PauseMenu->LoadMenuTree(PauseMenuTree);
		}
	}

	if(IsValid(InstructionsWidgetClass))
	{
		UUserWidget* InstructionsWidget = CreateWidget<UUserWidget>(GetWorld(), InstructionsWidgetClass);
		InstructionsWidget->AddToViewport();
	}

	if(AVTPlayerController* PlayerController = Cast<AVTPlayerController>(PlayerOwner))
	{
		PlayerController->SetInputMode(FInputModeGameAndUI());
		PlayerController->GetPlayerState<AVTPlayerState>()->ScoreChanged.AddDynamic(this, &AVTHUD::ScoreChanged);
	}
}

void AVTHUD::ShowPause()
{
	if(IsValid(PauseWidget))
	{
		if(UMenuTreeWidget* PauseMenu = Cast<UMenuTreeWidget>(PauseWidget))
		{
			PauseMenu->Show();
		}
		else
		{
			PauseWidget->AddToViewport();
		}
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Pause widget bad :("));
	}
}

void AVTHUD::HidePause()
{
	if(IsValid(PauseWidget))
	{
		if(UMenuTreeWidget* PauseMenu = Cast<UMenuTreeWidget>(PauseWidget))
		{
			PauseMenu->Hide();
		}
		else
		{
			PauseMenu->RemoveFromViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No pause widget :("));
	}
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
