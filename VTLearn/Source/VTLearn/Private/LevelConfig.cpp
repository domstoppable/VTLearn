#include "LevelConfig.h"
#include "PhoneticPhrase.h"

#include "Kismet/GameplayStatics.h"
#include "VTGameInstance.h"

int32 ULevelStatus::GetStarCount(int32 Score)
{
	if(Score == -1)
	{
		Score = HighScore;
	}

	int32 Stars = 0;
	for(Stars=0; Stars<LevelConfig.StarThresholds.Num(); Stars++)
	{
		if(LevelConfig.StarThresholds[Stars] > Score)
		{
			break;
		}
	}

	return Stars;
}

FVTLevelProgress ULevelStatus::GetProgress(const UObject* WorldContextObject)
{
	if(UVTGameInstance* GameInstance = Cast<UVTGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject)))
	{
		return GameInstance->LoadedSave->GetLevelProgress(GroupName, LevelConfig.Name);
	}

	FVTLevelProgress Dummy;
	return Dummy;
}
