// Fill out your copyright notice in the Description page of Project Settings.


#include "VTPlayerController.h"

#include "VibeyItem.h"
#include "VibeyItemReceiver.h"
#include "VTLearnCharacter.h"
#include "VTLearnGameMode.h"
#include "VTPlayerState.h"
#include "VTGameInstance.h"
#include "VTHUD.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/Engine.h"


AVTPlayerController::AVTPlayerController()
{
	bShouldPerformFullTickWhenPaused = 1;
}

void AVTPlayerController::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("Controller Begin Play"));
	AVTLearnGameMode* GameMode = GetWorld()->GetAuthGameMode<AVTLearnGameMode>();
	if(GameMode)
	{
		GameMode->LevelTimedOut.AddDynamic(this, &AVTPlayerController::OnLevelTimedOut);
	}
}

void AVTPlayerController::OnLevelTimedOut()
{
	Super::Pause();
	if(AVTHUD* HUD = Cast<AVTHUD>(MyHUD))
	{
		HUD->ShowLevelComplete();
	}
}

void AVTPlayerController::TogglePause()
{
	if(GetWorld()->GetAuthGameMode<AVTLearnGameMode>()->RemainingTime > 0.0f)
	{
		Pause();
	}
}

void AVTPlayerController::Pause()
{
	Super::Pause();
	if(AVTHUD* HUD = Cast<AVTHUD>(MyHUD))
	{
		if(IsPaused())
		{
			HUD->ShowPause();
			FInputModeGameAndUI InputMode;
			if(IsValid(HUD->PauseWidget))
			{
				InputMode.SetWidgetToFocus(HUD->PauseWidget->TakeWidget());
				UE_LOG(LogTemp, Log, TEXT("Focus pause widget"));
			}
			SetInputMode(InputMode);
			bShowMouseCursor = false;
		}else{
			HUD->HidePause();
			SetInputMode(FInputModeGameAndUI());
			bShowMouseCursor = false;
		}
	}
	PauseChanged.Broadcast(IsPaused());
}

void AVTPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
}

void AVTPlayerController::BindInputActions()
{
	InputComponent->BindAction("Jump", IE_Pressed, this, &AVTPlayerController::OnJump);
	InputComponent->BindAction("Grab", IE_Pressed, this, &AVTPlayerController::OnGrab);
	InputComponent->BindAction("Interact", IE_Pressed, this, &AVTPlayerController::OnInteract);
	InputComponent->BindAction("Revibe", IE_Pressed, this, &AVTPlayerController::OnRevibe);
	InputComponent->BindAction("TogglePause", IE_Pressed, this, &AVTPlayerController::TogglePause);

	InputComponent->BindAxis("MoveUp", this, &AVTPlayerController::OnMoveUp);
	InputComponent->BindAxis("MoveRight", this, &AVTPlayerController::OnMoveRight);

	Super::BindInputActions();
}

FVector AVTPlayerController::GetForward2D()
{
	return PlayerCameraManager->GetViewTarget()->GetActorForwardVector().GetSafeNormal2D();
}

void AVTPlayerController::OnMoveUp(float Value)
{
	if(IsPaused())
	{
		return;
	}

	APawn* PlayerPawn = GetPawn();
	if(!IsValid(PlayerPawn))
	{
		return;
	}

	const FVector Forward = GetForward2D();
	PlayerPawn->AddMovementInput(Forward, Value);
}

void AVTPlayerController::OnMoveRight(float Value)
{
	if(IsPaused())
	{
		return;
	}

	APawn* PlayerPawn = GetPawn();
	if(!IsValid(PlayerPawn))
	{
		return;
	}

	const FVector Right = GetForward2D().RotateAngleAxis(90, FVector(0.0f, 0.0f, 1.0f));
	PlayerPawn->AddMovementInput(Right, Value);
}

void AVTPlayerController::OnJump()
{
	if(IsPaused())
	{
		return;
	}

	if(ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn()))
	{
		PlayerCharacter->Jump();
	}
}

void AVTPlayerController::OnGrab()
{
	if(IsPaused())
	{
		return;
	}

	APawn* PlayerPawn = GetPawn();
	if(!IsValid(PlayerPawn))
	{
		return;
	}

	if(IsValid(HeldItem))
	{
		DropItem();
	}else{
		TArray<AActor*> ActorsInReach;
		PlayerPawn->GetOverlappingActors(ActorsInReach, TSubclassOf<AVibeyItem>());
		UE_LOG(LogTemp, Log, TEXT("Attempting to hold - %d actors nearby"), ActorsInReach.Num());
		for(auto Actor : ActorsInReach)
		{
			UE_LOG(LogTemp, Log, TEXT("Attempting to hold %s"), *Actor->GetName());
			if(HoldItem(Actor)){
				UE_LOG(LogTemp, Log, TEXT("I got it"));
				break;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("It cannot be held"));
			}
		}
	}
}

bool AVTPlayerController::CanInteract()
{
	// @TODO: Implement
	return true;
}

void AVTPlayerController::OnInteract()
{
	if(IsPaused())
	{
		return;
	}

	// @TODO: Implement
}

bool AVTPlayerController::CanRevibe()
{
	return IsValid(HeldItem);
}

void AVTPlayerController::OnRevibe()
{
	if(IsPaused())
	{
		return;
	}

	// @TODO: Implement
}

bool AVTPlayerController::CanHold(AActor* Actor)
{
	if(!IsValid(Actor))
	{
		return false;
	}

	if(AVibeyItem* Item = Cast<AVibeyItem>(Actor))
	{
		return Item->bGrabbable;
	}

	return false;
}

bool AVTPlayerController::HoldItem(AActor* Item)
{
	APawn* PlayerPawn = GetPawn();
	if(!IsValid(PlayerPawn))
	{
		return false;
	}

	if(!CanHold(Item))
	{
		return false;
	}

	if(AVibeyItem* VibeyItem = Cast<AVibeyItem>(Item))
	{
		if(UVTGameInstance* GameInstance = Cast<UVTGameInstance>(GetGameInstance()))
		{
			if(IsValid(GameInstance->VTDevice))
			{
				GameInstance->VTDevice->PlayPhrase(VibeyItem->Phrase);
			}
		}
	}

	HeldItem = Item;
	if(AVTLearnCharacter* PlayerCharacter = Cast<AVTLearnCharacter>(PlayerPawn))
	{
		PlayerCharacter->ItemGrabbed(HeldItem);
	}

	return true;
}

void AVTPlayerController::DropItem()
{
	APawn* PlayerPawn = GetPawn();
	if(!IsValid(PlayerPawn))
	{
		return;
	}

	if(AVTLearnCharacter* PlayerCharacter = Cast<AVTLearnCharacter>(PlayerPawn))
	{
		PlayerCharacter->ItemDropped(HeldItem);
	}
	AVibeyItem* Item = Cast<AVibeyItem>(HeldItem);
	HeldItem = nullptr;

	TArray<AActor*> ActorsInReach;
	PlayerPawn->GetOverlappingActors(ActorsInReach, TSubclassOf<AVibeyItemReceiver>());

	if(IsValid(Item) && ActorsInReach.Num() > 0)
	{
		for(AActor* ReachableActor : ActorsInReach)
		{
			if(AVibeyItemReceiver* Receiver = Cast<AVibeyItemReceiver>(ReachableActor))
			{
				Receiver->ReceiveItem(Item);
				break;
			}
		}
	}
}

AVTPlayerState* AVTPlayerController::GetVTPlayerState()
{
	return GetPlayerState<AVTPlayerState>();
}

AVTPlayerController* AVTPlayerController::GetVTPlayerController(UObject* WorldContextObject)
{
	return Cast<AVTPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
}
