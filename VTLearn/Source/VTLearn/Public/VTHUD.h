// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/WidgetComponent.h"

#include "VTHUDWidget.h"
#include "2D/MenuTree.h"
#include "2D/MenuTreeWidget.h"

#include "VTHUD.generated.h"

UCLASS()
class VTLEARN_API AVTHUD : public AHUD
{
	GENERATED_BODY()

public:
	AVTHUD();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UVTHUDWidget> StatsWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> InstructionsWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> LevelCompleteWidgetClass;

	UPROPERTY(BlueprintReadWrite)
	UVTHUDWidget* StatsWidget;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* PauseWidget;

	UFUNCTION(BlueprintCallable)
	void ShowPause();

	UFUNCTION(BlueprintCallable)
	void HidePause();

	UFUNCTION(BlueprintCallable)
	void ScoreChanged(int32 Delta, int32 Total);

	UFUNCTION(BlueprintCallable)
	void ShowLevelComplete();

};
