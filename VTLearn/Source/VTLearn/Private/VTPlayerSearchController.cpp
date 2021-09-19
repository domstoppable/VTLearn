// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VTPlayerSearchController.h"

#include "VTSearchGameMode.h"
#include "VTGameInstance.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"

void AVTPlayerSearchController::BeginPlay()
{
	Super::BeginPlay();
}

void AVTPlayerSearchController::OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnPlayerBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(AVibeyItem* Item = Cast<AVibeyItem>(OtherActor))
	{
		if(Item->bGrabbable)
		{
			UVTGameInstance* GameInstance = UVTGameInstance::GetVTGameInstance(this);
			if(IsValid(GameInstance->VTDevice))
			{
				if(IsValid(Item->Phrase))
				{
					GameInstance->VTDevice->PlayPhrase(Item->Phrase);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("VibeyItem has invalid phrase (%s)"), *Item->GetName());
				}
			}
		}
	}
}

bool AVTPlayerSearchController::HoldItem(AActor* Item)
{
	UE_LOG(LogTemp, Log, TEXT("AVTPlayerSearchController::HoldItem"));
	if(!CanHold(Item))
	{
		return false;
	}

	if(AVibeyItem* VibeyItem = Cast<AVibeyItem>(Item))
	{
		AVTSearchGameMode* GameMode = GetWorld()->GetAuthGameMode<AVTSearchGameMode>();
		VibeyItem->bIsGood = GameMode->Matcher->Match(VibeyItem->Phrase);
		VibeyItem->bGrabbable = false;
		VibeyItem->Grabbed(GetPawn());

		GetPlayerState<AVTPlayerState>()->OnItemAttempted(VibeyItem->Phrase, VibeyItem->bIsGood);

		Reachables.Remove(VibeyItem);

		return true;
	}

	return false;
}

bool AVTPlayerSearchController::CanRevibe()
{
	return CanInteract();
}

void AVTPlayerSearchController::OnRevibe()
{
	if(IsPaused() || !CanRevibe())
	{
		return;
	}

	if(UVTGameInstance* GameInstance = Cast<UVTGameInstance>(GetGameInstance()))
	{
		if(IsValid(GameInstance->VTDevice))
		{
			AVibeyItem* VibeyItem = GetFirstReachableOfClass<AVibeyItem>();
			GameInstance->VTDevice->PlayPhrase(VibeyItem->Phrase);
		}
	}
}
