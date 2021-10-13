// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VTBlueprintLibrary.h"

#include "VTGameInstance.h"

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
