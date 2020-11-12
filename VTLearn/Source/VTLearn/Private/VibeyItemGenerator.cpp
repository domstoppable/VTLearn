// Fill out your copyright notice in the Description page of Project Settings.


#include "VibeyItemGenerator.h"
#include "Math/UnrealMathUtility.h"


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
	if(Phrases.Num() == 0)
	{
		return nullptr;
	}

	return Phrases[FMath::RandRange(0, Phrases.Num()-1)];
}
