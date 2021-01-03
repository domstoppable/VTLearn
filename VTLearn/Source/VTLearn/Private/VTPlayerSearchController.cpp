// Fill out your copyright notice in the Description page of Project Settings.

#include "VTPlayerSearchController.h"

#include "VTSearchGameMode.h"
#include "VTGameInstance.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"

void AVTPlayerSearchController::BeginPlay()
{
	Super::BeginPlay();

	TArray<UActorComponent*> Components = GetPawn()->GetComponentsByTag(UPrimitiveComponent::StaticClass(), FName(TEXT("interaction")));
	if(Components.Num() > 0)
	{
		Cast<UPrimitiveComponent>(Components[0])->OnComponentBeginOverlap.AddDynamic(this, &AVTPlayerSearchController::OnPlayerBeginOverlap);
	}
}

void AVTPlayerSearchController::OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(AVibeyItem* Item = Cast<AVibeyItem>(OtherActor))
	{
		ItemInReach.Broadcast(Item);

		UVTGameInstance* GameInstance = UVTGameInstance::GetVTGameInstance(this);
		if(IsValid(GameInstance->VTDevice))
		{
			GameInstance->VTDevice->PlayPhrase(Item->Phrase);
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

		if(VibeyItem->bIsGood)
		{
			AwardPlayer();
		}
		else
		{
			PunishPlayer();
		}


		return true;
	}

	return false;
}