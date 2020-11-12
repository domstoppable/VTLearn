// Fill out your copyright notice in the Description page of Project Settings.


#include "VibeyItemReceiver.h"


// Sets default values
AVibeyItemReceiver::AVibeyItemReceiver()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AVibeyItemReceiver::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AVibeyItemReceiver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVibeyItemReceiver::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Matcher = NewObject<UPhoneSequenceMatcher>();
}
