// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VTSaveGame.h"
#include "LevelConfig.h"
#include "VTGameInstance.h"

#include "Kismet/GameplayStatics.h"

#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

TArray<FString> UVTSaveGame::GetAllSaveGameSlotNames()
{
	// https://answers.unrealengine.com/questions/145598/is-there-a-way-to-get-all-savegames-in-bp.html
	//////////////////////////////////////////////////////////////////////////////
	class FFindSavesVisitor : public IPlatformFile::FDirectoryVisitor
	{
	public:
		FFindSavesVisitor() {}

		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory)
		{
			if (!bIsDirectory)
			{
				FString FullFilePath(FilenameOrDirectory);
				if (FPaths::GetExtension(FullFilePath) == TEXT("sav"))
				{
					FString CleanFilename = FPaths::GetBaseFilename(FullFilePath);
					CleanFilename = CleanFilename.Replace(TEXT(".sav"), TEXT(""));
					SavesFound.Add(CleanFilename);
				}
			}
			return true;
		}
		TArray<FString> SavesFound;
	};
	//////////////////////////////////////////////////////////////////////////////

	TArray<FString> Saves;
	const FString SavesFolder = FPaths::ProjectSavedDir() + TEXT("SaveGames");

	if (!SavesFolder.IsEmpty())
	{
		FFindSavesVisitor Visitor;
		FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*SavesFolder, Visitor);
		Saves = Visitor.SavesFound;
	}

	Saves.Sort();

	return Saves;
}

TArray<UVTSaveGame*> UVTSaveGame::LoadVTSaveGames(UObject* WorldContextObject)
{
	TArray<UVTSaveGame*> Saves;
	TArray<FString> Names = GetAllSaveGameSlotNames();

	USaveGame* DefaultSaveGame = UGameplayStatics::CreateSaveGameObject(UVTSaveGame::StaticClass());
	if(UVTSaveGame* DefaultVTSaveGame = Cast<UVTSaveGame>(DefaultSaveGame))
	{
		UVTGameInstance* GameInstance = UVTGameInstance::GetVTGameInstance(WorldContextObject);
		DefaultVTSaveGame->PID = GameInstance->DefaultPID;
		DefaultVTSaveGame->Username = GameInstance->DefaultUsername;

		if(!Names.Contains(DefaultVTSaveGame->GetSlotName()))
		{
			UGameplayStatics::SaveGameToSlot(DefaultVTSaveGame, DefaultVTSaveGame->GetSlotName(), 0);
			Names.Add(DefaultVTSaveGame->GetSlotName());
		}
	}

	for(FString Name : Names)
	{
		UE_LOG(LogTemp, Log, TEXT("Loading save %s"), *Name);
		USaveGame* Save = UGameplayStatics::LoadGameFromSlot(Name, 0);
		Saves.Add((UVTSaveGame*)Save);
	}

	return Saves;
}

FString UVTSaveGame::GetSlotName()
{
	return FString::Printf(TEXT("%04d - %s"), PID, *Username);
}

FVTLevelProgress UVTSaveGame::GetLevelProgress(FString GroupName, FString LevelName)
{
	for(FVTLevelProgress LevelProgress : Progress)
	{
		if(LevelProgress.GroupName == GroupName && LevelProgress.LevelName == LevelName)
		{
			return LevelProgress;
		}
	}

	return FVTLevelProgress();
}

void UVTSaveGame::SetHighScore(FString GroupName, FString LevelName, int32 Score)
{

	for(FVTLevelProgress &LevelProgress : Progress)
	{
		if(LevelProgress.GroupName == GroupName && LevelProgress.LevelName == LevelName)
		{
			if(Score > LevelProgress.HighScore)
			{
				LevelProgress.HighScore = Score;
			}
			return;
		}
	}

	FVTLevelProgress LevelProgress;
	LevelProgress.GroupName = GroupName;
	LevelProgress.LevelName = LevelName;
	LevelProgress.HighScore = Score;

	Progress.Add(LevelProgress);
}

int32 UVTSaveGame::GetGroupStarCount(ULevelGroupStatus* GroupStatus)
{
	int32 Stars = 0;
	for(ULevelStatus* LevelStatus : GroupStatus->LevelStatuses)
	{
		Stars += GetLevelStarCount(LevelStatus);
	}

	return Stars;
}

int32 UVTSaveGame::GetLevelStarCount(ULevelStatus* LevelStatus)
{
	FVTLevelProgress LevelProgress = GetLevelProgress(LevelStatus->GroupName, LevelStatus->LevelConfig.Name);

	return LevelStatus->GetStarCount(LevelProgress.HighScore);
}
