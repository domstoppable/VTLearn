// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VibeyItem.h"

// Sets default values
AVibeyItem::AVibeyItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVibeyItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AVibeyItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVibeyItem::SetPhrase_Implementation(UPhoneticPhrase* NewPhrase)
{
	Phrase = NewPhrase;
	if(IsValid(NewPhrase))
	{
		UE_LOG(LogTemp, Log, TEXT("VibeyItem Phrase has been set to %s"), *NewPhrase->WrittenText);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("VibeyItem Phrase has been set to something invalid?"));
	}


}