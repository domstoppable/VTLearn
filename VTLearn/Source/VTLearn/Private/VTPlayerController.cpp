// Fill out your copyright notice in the Description page of Project Settings.


#include "VTPlayerController.h"

#include "VibeyItem.h"
#include "VTLearnCharacter.h"
#include "VibeyItemReceiver.h"
#include "VTNetworkClient.h"
#include "VTHUD.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

AVTPlayerController::AVTPlayerController()
{
	VTDevice = NewObject<UVTNetworkClient>();
	bShouldPerformFullTickWhenPaused = 1;
}

void AVTPlayerController::BeginPlay()
{
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, [this]() {
		TogglePause();
	}, 0.1f, false);
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
			SetInputMode(FInputModeGameAndUI());
			bShowMouseCursor = true;
		}else{
			HUD->HidePause();
			SetInputMode(FInputModeGameOnly());
			bShowMouseCursor = false;
		}
	}
}

void AVTPlayerController::ConnectToDevice(FString IP, int32 Port)
{
	FVTNetworkClientStatusChangedDelegate ConnectDelegate;
	ConnectDelegate.BindDynamic(this, &AVTPlayerController::OnDeviceConnected);

	FVTNetworkClientStatusChangedDelegate DisconnectDelegate;
	DisconnectDelegate.BindDynamic(this, &AVTPlayerController::OnDeviceDisconnected);

	if(VTDevice->ConnectionState != EDeviceConnectionState::Connected)
	{
		VTDevice->Connect(IP, Port, ConnectDelegate, DisconnectDelegate);
	}
}

void AVTPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(VTDevice->ConnectionState != EDeviceConnectionState::Disconnected)
	{
		VTDevice->Disconnect();
	}
}

void AVTPlayerController::OnDeviceConnected()
{
	DeviceConnected.Broadcast();
}

void AVTPlayerController::OnDeviceDisconnected()
{
	DeviceDisconnected.Broadcast();
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
	const FVector Direction = FVector(1.0f, 0.0f, 0.0f);
	GetPawn()->AddMovementInput(Direction, Value);
}

void AVTPlayerController::OnMoveRight(float Value)
{
	const FVector Direction = FVector(0.0f, 1.0f, 0.0f);
	GetPawn()->AddMovementInput(Direction, Value);
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

	if(IsValid(HeldItem))
	{
		DropItem();
	}else{
		TArray<AActor*> ActorsInReach;
		GetPawn()->GetOverlappingActors(ActorsInReach, TSubclassOf<AVibeyItem>());
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
	if(!CanHold(Item))
	{
		return false;
	}

	if(AVibeyItem* VibeyItem = Cast<AVibeyItem>(Item))
	{
		VTDevice->PlayPhrase(VibeyItem->Phrase);
	}

	HeldItem = Item;
	if(AVTLearnCharacter* Character = Cast<AVTLearnCharacter>(GetPawn()))
	{
		Character->ItemGrabbed(HeldItem);
	}

	return true;
}

void AVTPlayerController::DropItem()
{
	if(AVTLearnCharacter* Character = Cast<AVTLearnCharacter>(GetPawn()))
	{
		Character->ItemDropped(HeldItem);
	}
	AVibeyItem* Item = Cast<AVibeyItem>(HeldItem);
	HeldItem = nullptr;

	TArray<AActor*> ActorsInReach;
	GetPawn()->GetOverlappingActors(ActorsInReach, TSubclassOf<AVibeyItemReceiver>());
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