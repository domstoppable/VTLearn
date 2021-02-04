// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "VTLearnEd.h"

IMPLEMENT_MODULE(FVTLearnEdModule, VTLearnEd)

#define LOCTEXT_NAMESPACE "FVTLearnEdModule"

void FVTLearnEdModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("VTLearnEd compiled %s at %s"), TEXT(__DATE__), TEXT(__TIME__));
}

void FVTLearnEdModule::ShutdownModule()
{

}
