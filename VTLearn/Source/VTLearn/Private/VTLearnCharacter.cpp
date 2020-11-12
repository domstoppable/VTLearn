// Fill out your copyright notice in the Description page of Project Settings.


#include "VTLearnCharacter.h"


AVTLearnCharacter::AVTLearnCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVTLearnCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AVTLearnCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVTLearnCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveUp", this, &AVTLearnCharacter::MoveUp);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVTLearnCharacter::MoveRight);
}

void AVTLearnCharacter::MoveUp(float Value)
{
	const FVector Direction = FVector(1.0f, 0.0f, 0.0f);
	AddMovementInput(Direction, Value);
}

void AVTLearnCharacter::MoveRight(float Value)
{
	const FVector Direction = FVector(0.0f, 1.0f, 0.0f);
	AddMovementInput(Direction, Value);
}
