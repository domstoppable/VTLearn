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
#include "AudioDevice.h"

#include "Engine/Engine.h"


AVTPlayerController::AVTPlayerController()
{
	bShouldPerformFullTickWhenPaused = 1;

	ConstructorHelpers::FObjectFinder<USoundWave> SoundFinder(TEXT("SoundWave'/Game/Effects/Audio/noise.noise'"));
	if(SoundFinder.Succeeded())
	{
		NoiseWave = SoundFinder.Object;
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Could not find SoundWave for noise"));
	}

}

void AVTPlayerController::BeginPlay()
{
	if(AVTLearnGameMode* GameMode = GetWorld()->GetAuthGameMode<AVTLearnGameMode>())
	{
		GameMode->LevelTimedOut.AddDynamic(this, &AVTPlayerController::OnLevelTimedOut);
	}

	FSlateApplication::Get().OnApplicationActivationStateChanged()
		.AddUObject(this, &AVTPlayerController::OnWindowFocusChanged);
}

void AVTPlayerController::OnWindowFocusChanged(bool bIsFocused)
{
	if(!bIsFocused && !IsPaused())
	{
		Pause();
	}
}

void AVTPlayerController::OnLevelTimedOut()
{
	Super::Pause();
	if(AVTHUD* HUD = Cast<AVTHUD>(MyHUD))
	{
		HUD->ShowLevelComplete();
	}

	if(NoiseAudio)
	{
		NoiseAudio->Stop();
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

	if(!NoiseAudio && NoiseWave)
	{
		FAudioDevice::FCreateComponentParams Params(GetWorld());
		NoiseAudio = FAudioDevice::CreateComponent(NoiseWave, Params);
		if(NoiseAudio)
		{
			NoiseAudio->bIsUISound = true;
			NoiseAudio->VolumeMultiplier = 1.0f;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create noise AudioComponent"));
		}
	}

	if(NoiseAudio)
	{
		if(IsPaused())
		{
			NoiseAudio->Stop();
		}else{
			NoiseAudio->Play(0.0f);
		}

	}
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

	if(AVibeyItem* Item = Cast<AVibeyItem>(HeldItem))
	{
		if(UVTGameInstance* GameInstance = Cast<UVTGameInstance>(GetGameInstance()))
		{
			if(IsValid(GameInstance->VTDevice))
			{
				GameInstance->VTDevice->PlayPhrase(Item->Phrase);
			}
		}
	}
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
