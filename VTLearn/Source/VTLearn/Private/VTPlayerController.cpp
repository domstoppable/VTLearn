// Fill out your copyright notice in the Description page of Project Settings.


#include "VTPlayerController.h"

#include "VibeyItem.h"
#include "VTLearnCharacter.h"
#include "VibeyItemReceiver.h"
#include "VTNetworkClient.h"
#include "VTHUD.h"
#include "VTGameInstance.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

#include "Engine/Engine.h"


AVTPlayerController::AVTPlayerController()
{
	bShouldPerformFullTickWhenPaused = 1;
}

void AVTPlayerController::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("Controller Begin Play"));
}

void AVTPlayerController::TogglePause()
{
	Pause();
}

void AVTPlayerController::Pause()
{
	super::Pause();
	if(AVTHUD* HUD = Cast<AVTHUD>(MyHUD))
	{
		if(IsPaused())
		{
			HUD->ShowPause();
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(HUD->PauseWidget->TakeWidget());
			SetInputMode(InputMode);
			bShowMouseCursor = true;
		}else{
			HUD->HidePause();
			SetInputMode(FInputModeGameAndUI());
			bShowMouseCursor = false;
		}
	}
}

void AVTPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
}

void AVTPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &AVTPlayerController::OnJump);
	InputComponent->BindAction("Grab", IE_Pressed, this, &AVTPlayerController::OnGrab);
	InputComponent->BindAction("Interact", IE_Pressed, this, &AVTPlayerController::OnInteract);
	InputComponent->BindAction("TogglePause", IE_Pressed, this, &AVTPlayerController::TogglePause);

	InputComponent->BindAxis("MoveUp", this, &AVTPlayerController::OnMoveUp);
	InputComponent->BindAxis("MoveRight", this, &AVTPlayerController::OnMoveRight);
}

void AVTPlayerController::OnMoveUp(float Value)
{
	APawn* Pawn = GetPawn();
	if(!IsValid(Pawn))
	{
		return;
	}

	const FVector Direction = FVector(1.0f, 0.0f, 0.0f);
	Pawn->AddMovementInput(Direction, Value);
}

void AVTPlayerController::OnMoveRight(float Value)
{
	APawn* Pawn = GetPawn();
	if(!IsValid(Pawn))
	{
		return;
	}

	const FVector Direction = FVector(0.0f, 1.0f, 0.0f);
	Pawn->AddMovementInput(Direction, Value);
}

void AVTPlayerController::OnJump()
{
	if(ACharacter* Character = Cast<ACharacter>(GetPawn()))
	{
		Character->Jump();
	}
}

void AVTPlayerController::OnGrab()
{
	APawn* Pawn = GetPawn();
	if(!IsValid(Pawn))
	{
		return;
	}

	if(IsValid(HeldItem))
	{
		DropItem();
	}else{
		TArray<AActor*> ActorsInReach;
		Pawn->GetOverlappingActors(ActorsInReach, TSubclassOf<AVibeyItem>());
		for(auto Actor : ActorsInReach)
		{
			if(HoldItem(Actor)){
				break;
			}
		}
	}
}

void AVTPlayerController::OnInteract()
{

}

bool AVTPlayerController::CanHold(AActor* Item)
{
	return IsValid(Item) && Item->IsA(AVibeyItem::StaticClass());
}

bool AVTPlayerController::HoldItem(AActor* Item)
{
	APawn* Pawn = GetPawn();
	if(!IsValid(Pawn))
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
			GameInstance->VTDevice->PlayPhrase(VibeyItem->Phrase);
		}
	}

	HeldItem = Item;
	if(AVTLearnCharacter* Character = Cast<AVTLearnCharacter>(Pawn))
	{
		Character->ItemGrabbed(HeldItem);
	}

	return true;
}

void AVTPlayerController::DropItem()
{
	APawn* Pawn = GetPawn();
	if(!IsValid(Pawn))
	{
		return;
	}

	if(AVTLearnCharacter* Character = Cast<AVTLearnCharacter>(Pawn))
	{
		Character->ItemDropped(HeldItem);
	}
	AVibeyItem* Item = Cast<AVibeyItem>(HeldItem);
	HeldItem = nullptr;

	TArray<AActor*> ActorsInReach;
	Pawn->GetOverlappingActors(ActorsInReach, TSubclassOf<AVibeyItemReceiver>());
	if(IsValid(Item) && ActorsInReach.Num() > 0)
	{
		if(AVibeyItemReceiver* Receiver = Cast<AVibeyItemReceiver>(ActorsInReach[0]))
		{
			Receiver->ReceiveItem(Item);
		}
	}

}

int32 AVTPlayerController::IncreaseScore()
{
	Score += 10;
	ScoreChanged.Broadcast(Score);
	return Score;
}

int32 AVTPlayerController::DecreaseLives()
{
	Lives--;
	LivesChanged.Broadcast(Lives);

	return Lives;
}
