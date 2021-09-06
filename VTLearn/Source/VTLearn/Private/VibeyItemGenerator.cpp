// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VibeyItemGenerator.h"

#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AVibeyItemGenerator::AVibeyItemGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVibeyItemGenerator::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AVibeyItemGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UPhoneticPhrase* AVibeyItemGenerator::RandomPhrase()
{
	if(CurrentPool.Num() == 0)
	{
		CurrentPool.Append(Phrases);
	}
	if(CurrentPool.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AVibeyItemGenerator::RandomPhrase No items to put in pool!"));
		return nullptr;
	}

	int32 PhraseIdx = FMath::RandRange(0, CurrentPool.Num()-1);
	UPhoneticPhrase* Phrase = CurrentPool[PhraseIdx];
	CurrentPool.RemoveAt(PhraseIdx);

	return Phrase;
}

void AVibeyItemGenerator::AddToPhraseBank(TArray<UPhoneticPhrase*> MorePhrases)
{
	Phrases.Append(MorePhrases);
	UE_LOG(LogTemp, Log, TEXT("Item generator `%s` now has %d phrases"), *GetName(), Phrases.Num());
}