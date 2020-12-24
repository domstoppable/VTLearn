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
			}
			SetInputMode(InputMode);
			bShowMouseCursor = true;
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

void AVTPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &AVTPlayerController::OnJump);
	InputComponent->BindAction("Grab", IE_Pressed, this, &AVTPlayerController::OnGrab);
	InputComponent->BindAction("Interact", IE_Pressed, this, &AVTPlayerController::OnInteract);
	InputComponent->BindAction("Revibe", IE_Pressed, this, &AVTPlayerController::OnRevibe);
	InputComponent->BindAction("TogglePause", IE_Pressed, this, &AVTPlayerController::TogglePause);

	InputComponent->BindAxis("MoveUp", this, &AVTPlayerController::OnMoveUp);
	InputComponent->BindAxis("MoveRight", this, &AVTPlayerController::OnMoveRight);
}

FVector AVTPlayerController::GetForward2D()
{
	return FVector(1.0f, 0.0f, 0.0f);
}

void AVTPlayerController::OnMoveUp(float Value)
{
	APawn* Pawn = GetPawn();
	if(!IsValid(Pawn))
	{
		return;
	}

	const FVector Forward = GetForward2D();
	Pawn->AddMovementInput(Forward, Value);
}

void AVTPlayerController::OnMoveRight(float Value)
{
	APawn* Pawn = GetPawn();
	if(!IsValid(Pawn))
	{
		return;
	}

	const FVector Right = GetForward2D().RotateAngleAxis(90, FVector(0.0f, 0.0f, 1.0f));
	Pawn->AddMovementInput(Right, Value);
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
		UE_LOG(LogTemp, Log, TEXT("Attempting to hold - %d actors nearby"), ActorsInReach.Num());
		for(auto Actor : ActorsInReach)
		{
			UE_LOG(LogTemp, Log, TEXT("Attempting to hold %s"), *Actor->GetActorLabel());
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
	// @TODO: Implement
}

bool AVTPlayerController::CanRevibe()
{
	return IsValid(HeldItem);
}

void AVTPlayerController::OnRevibe()
{
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
			if(IsValid(GameInstance->VTDevice))
			{
				GameInstance->VTDevice->PlayPhrase(VibeyItem->Phrase);
			}
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

int32 AVTPlayerController::AdjustScore(int32 Amount)
{
	int32 NewScore = GetVTPlayerState()->GetScore() + Amount;
	if(NewScore < 0){
		NewScore = 0;
	}
	GetVTPlayerState()->SetScore(NewScore);
	ScoreChanged.Broadcast(Amount, NewScore);

	return NewScore;
}

int32 AVTPlayerController::AwardPlayer()
{
	GetVTPlayerState()->CorrectCount++;
	return AdjustScore(GetWorld()->GetAuthGameMode<AVTLearnGameMode>()->PointsForCorrect);
}

int32 AVTPlayerController::PunishPlayer()
{
	GetVTPlayerState()->IncorrectCount++;
	return AdjustScore(GetWorld()->GetAuthGameMode<AVTLearnGameMode>()->PointsForIncorrect);
}

AVTPlayerState* AVTPlayerController::GetVTPlayerState()
{
	return GetPlayerState<AVTPlayerState>();
}
