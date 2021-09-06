// (c) 2021 Dominic Canare <dom@dominiccanare.com>

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
