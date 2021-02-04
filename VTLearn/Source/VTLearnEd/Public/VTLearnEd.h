// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FVTLearnEdModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
