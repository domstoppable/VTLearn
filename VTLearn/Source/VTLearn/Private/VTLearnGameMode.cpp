#include "VTLearnGameMode.h"

#include "VTPlayerController.h"
#include "VTLearnCharacter.h"
#include "VTHUD.h"

AVTLearnGameMode::AVTLearnGameMode()
{
	PlayerControllerClass = AVTPlayerController::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/GameCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}else{
		DefaultPawnClass = AVTLearnCharacter::StaticClass();
	}

	static ConstructorHelpers::FClassFinder<AHUD> HUDBPClass(TEXT("/Game/UI/VTHUDBP"));
	if (HUDBPClass.Class != NULL)
	{
		HUDClass = HUDBPClass.Class;
	}else{
		HUDClass = AVTHUD::StaticClass();
	}
}
