// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "VTLearnGameModeBase.h"

AVTLearnGameModeBase::AVTLearnGameModeBase()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/GameCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
