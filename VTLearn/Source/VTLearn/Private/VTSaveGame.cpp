#include "VTSaveGame.h"

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
			LevelProgress.HighScore = Score;
			return;
		}
	}

	FVTLevelProgress LevelProgress;
	LevelProgress.GroupName = GroupName;
	LevelProgress.LevelName = LevelName;
	LevelProgress.HighScore = Score;

	Progress.Add(LevelProgress);
}
