// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VTBlueprintLibrary.h"

#include "VTGameInstance.h"
#include "PsydekickBPLibrary.h"

#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json.h"
#include "Serialization/JsonReader.h"

void UVTBlueprintLibrary::DownloadHighScores(const UObject* WorldContextObject, const FDownloadFinished& DownloadFinished)
{
	UVTGameInstance* GameInstance = UVTGameInstance::GetVTGameInstance(WorldContextObject);
	if(GameInstance->LeaderboardServer.Equals(""))
	{
		UE_LOG(LogTemp, Error, TEXT("Leaderboard server not configured!"));
		return;
	}

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	FString Url = FString::Printf(TEXT("%s/api/scores"), *GameInstance->LeaderboardServer);
	Request->SetURL(Url);
	Request->SetVerb("GET");

	UE_LOG(LogTemp, Log, TEXT("Downloading high scores from %s..."), *Url);
	Request->OnProcessRequestComplete().BindLambda([DownloadFinished, GameInstance](FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success){
		TArray<FString> UserIDs;
		TArray<int32> Scores;

		if(Success && Response.IsValid())
		{
			if (!EHttpResponseCodes::IsOk(Response->GetResponseCode()))
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to download scores! [%d]"), Response->GetResponseCode());
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Processing high scores..."));

				TSharedPtr<FJsonObject> ParsedResponse;
				TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString());
				if (FJsonSerializer::Deserialize(JsonReader, ParsedResponse))
				{
					TArray<TSharedPtr<FJsonValue>> ScoreValues = ParsedResponse->GetArrayField("scores");
					for(auto& ScoreValue : ScoreValues)
					{
						TSharedPtr<FJsonObject> ScoreValueObj = ScoreValue->AsObject();
						UserIDs.Emplace(ScoreValueObj->GetStringField("user_id"));
						Scores.Emplace(ScoreValueObj->GetIntegerField("score"));
					}
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to download scores!"));
		}


		if(IsValid(GameInstance->LoadedSave))
		{
			FString MyUserID = FString::Printf(TEXT("%d"), GameInstance->LoadedSave->PID);

			int32 MyIdx = -1;
			if(UserIDs.Find(MyUserID, MyIdx))
			{
				Scores[MyIdx] = GameInstance->LoadedSave->TotalScore;
			}
			else
			{
				UserIDs.Emplace(MyUserID);
				Scores.Emplace(GameInstance->LoadedSave->TotalScore);
			}
		}

		DownloadFinished.ExecuteIfBound(UserIDs, Scores);
	});

	Request->ProcessRequest();
}

FString UVTBlueprintLibrary::GetBuildDate()
{
	FString MacroDate = TEXT(__DATE__); // Jan 02 3456

	FString Year = *MacroDate.Mid(7, 4);
	FString Day = *MacroDate.Mid(4, 2).TrimStartAndEnd();

	int32 Month = 0;

	FString MonthAbbr = MacroDate.Left(3);
	if (MonthAbbr.Equals("Jan")) Month = 1;
	else if (MonthAbbr.Equals("Feb")) Month = 2;
	else if (MonthAbbr.Equals("Mar")) Month = 3;
	else if (MonthAbbr.Equals("Apr")) Month = 4;
	else if (MonthAbbr.Equals("May")) Month = 5;
	else if (MonthAbbr.Equals("Jun")) Month = 6;
	else if (MonthAbbr.Equals("Jul")) Month = 7;
	else if (MonthAbbr.Equals("Aug")) Month = 8;
	else if (MonthAbbr.Equals("Sep")) Month = 9;
	else if (MonthAbbr.Equals("Oct")) Month = 10;
	else if (MonthAbbr.Equals("Nov")) Month = 11;
	else if (MonthAbbr.Equals("Dec")) Month = 12;

	return FString::Printf(TEXT("%s-%02d-%02s"), *Year, Month, *Day);
}

FString UVTBlueprintLibrary::GetBuildTime()
{
	return TEXT(__TIME__);
}

FString UVTBlueprintLibrary::GetBuildString()
{
	FString Version = GetProjectSetting(TEXT("ProjectVersion"));
	FString Date = GetBuildDate().Replace(TEXT("-"), TEXT(""));
	FString Time = GetBuildTime().Replace(TEXT(":"), TEXT(""));

	return FString::Printf(TEXT("v%s_%s_%s"), *Version, *Date, *Time);
}

FString UVTBlueprintLibrary::GetProjectSetting(FString Name)
{
	FString Value;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		*Name,
		Value,
		GGameIni
	);

	return Value;
}