// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VTPlayerController.h"

#include "VibeyItem.h"
#include "VibeyItemReceiver.h"
#include "VTLearnCharacter.h"
#include "VTLearnGameMode.h"
#include "VTPlayerState.h"
#include "VTGameInstance.h"
#include "VTHUD.h"
#include "Interactable.h"

#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AudioDevice.h"
#include "GameFramework/CharacterMovementComponent.h" 

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
	Reachables = TArray<AActor*>();

	if(AVTLearnGameMode* GameMode = GetWorld()->GetAuthGameMode<AVTLearnGameMode>())
	{
		GameMode->LevelTimedOut.AddDynamic(this, &AVTPlayerController::OnLevelTimedOut);
	}

	FSlateApplication::Get().OnApplicationActivationStateChanged()
		.AddUObject(this, &AVTPlayerController::OnWindowFocusChanged);

	TArray<UActorComponent*> Components = GetPawn()->GetComponentsByTag(UPrimitiveComponent::StaticClass(), FName(TEXT("interaction")));
	if (Components.Num() > 0)
	{
		Cast<UPrimitiveComponent>(Components[0])->OnComponentBeginOverlap.AddDynamic(this, &AVTPlayerController::OnPlayerBeginOverlap);
		Cast<UPrimitiveComponent>(Components[0])->OnComponentEndOverlap.AddDynamic(this, &AVTPlayerController::OnPlayerEndOverlap);
	}
}

void AVTPlayerController::OnWindowFocusChanged(bool bIsFocused)
{
	if(!bIsFocused && !IsPaused())
	{
		Pause();
	}

	AppFocusChanged.Broadcast(bIsFocused);
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
			SetInputMode(FInputModeGameOnly());
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
	InputComponent->bBlockInput = false;
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
		for(auto Reachable : Reachables)
		{
			UE_LOG(LogTemp, Log, TEXT("Attempting to hold %s"), *Reachable->GetName());
			if(HoldItem(Reachable)){
				break;
			}
		}
	}
}

bool AVTPlayerController::CanInteract()
{
	if (IsValid(HeldItem))
	{
		return true;
	}

	for (AActor* Reachable : Reachables)
	{
		if (IsValid(Reachable))
		{
			if (AVibeyItem* Item = Cast<AVibeyItem>(Reachable))
			{
				if (!(IsValid(Item) && Item->bGrabbable))
				{
					continue;
				}
			}

			if (AVibeyItemReceiver* Receiver = Cast<AVibeyItemReceiver>(Reachable))
			{
				if(!IsValid(HeldItem))
				{
					continue;
				}
			}

			return true;
		}
	}

	return false;
}

void AVTPlayerController::OnInteract()
{
	if(IsPaused())
	{
		return;
	}
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

void AVTPlayerController::OnDash()
{
	if (IsPaused())
	{
		return;
	}

	APawn* PlayerPawn = GetPawn();
	if (!IsValid(PlayerPawn))
	{
		return;
	}

	if (UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(PlayerPawn->GetMovementComponent()))
	{
		FVector Impulse = PlayerPawn->GetActorForwardVector().GetSafeNormal2D() * DashSpeed;
		MovementComponent->AddImpulse(Impulse);
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

	AVibeyItemReceiver* Receiver = GetFirstReachableOfClass<AVibeyItemReceiver>();

	if(IsValid(Item) && IsValid(Receiver))
	{
		UE_LOG(LogTemp, Log, TEXT("Giving item <%s> to receiver <%s>"), *Item->GetName(), *Receiver->GetName());
		Receiver->ReceiveItem(Item);
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

void AVTPlayerController::OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		Reachables.Add(OtherActor);
		if (AVibeyItem* Item = Cast<AVibeyItem>(OtherActor))
		{
			ItemInReach.Broadcast(Item);
		}
	}
}

void AVTPlayerController::OnPlayerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Reachables.Remove(OtherActor);
}
