// Fill out your copyright notice in the Description page of Project Settings.


#include "VibeyLevelController.h"
#include "PhoneticPhrase.h"


#include "EngineUtils.h"
#include "VibeyItemGenerator.h"
#include "VibeyItemReceiver.h"
#include "Math/UnrealMathUtility.h"


// Sets default values
AVibeyLevelController::AVibeyLevelController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVibeyLevelController::BeginPlay()
{
	Super::BeginPlay();

	AllPhrases = UPhoneticPhrase::LoadPhrasesFromDirectory(VTTDirectory);

	UE_LOG(LogTemp, Log, TEXT("Starting controller with %d phrases"), AllPhrases.Num());

	for(AVibeyItemReceiver* ItemReceiver : Receivers)
	{
		if(AllPhrases.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Ran out of phrases!"));
			break;
		}

		bool alreadyAdded = true;
		while(alreadyAdded)
		{
			alreadyAdded = false;
			int Idx = FMath::RandRange(0, AllPhrases.Num()-1);
			for(auto Phrase : SelectedPhrases)
			{
				if(Phrase->WrittenText == AllPhrases[Idx]->WrittenText)
				{
					alreadyAdded = true;
					break;
				}
			}

			if(alreadyAdded)
			{
				continue;
			}

			SelectedPhrases.Emplace(AllPhrases[Idx]);
			ItemReceiver->SetMatchPhrase(AllPhrases[Idx]);

			UE_LOG(LogTemp, Log, TEXT("Added phrase %s"), *(AllPhrases[Idx]->WrittenText));
			AllPhrases.RemoveAt(Idx);
		}
	}

	for(AVibeyItemGenerator* ItemGenerator : Generators)
	{
		ItemGenerator->Phrases = SelectedPhrases;
	}

}

void AVibeyLevelController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
