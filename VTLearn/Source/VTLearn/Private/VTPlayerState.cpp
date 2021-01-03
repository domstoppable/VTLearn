// Fill out your copyright notice in the Description page of Project Settings.


#include "VTPlayerState.h"

#include "VTLearnGameMode.h"

int32 AVTPlayerState::OnItemAttempted(UPhoneticPhrase* Phrase, bool bCorrect)
{
	// Store attempt
	TMap<UPhoneticPhrase*, int32> *Map = bCorrect ? &CorrectCounts : &IncorrectCounts;

	int32 Count = Map->FindOrAdd(Phrase);
	Map->Add(Phrase, ++Count);

	// Adjust score
	AVTLearnGameMode* GameMode = GetWorld()->GetAuthGameMode<AVTLearnGameMode>();
	int32 PointDelta = bCorrect ? GameMode->PointsForCorrect : GameMode->PointsForIncorrect;
	int32 NewScore = GetScore() + PointDelta;
	if(NewScore < 0){
		NewScore = 0;
	}

	SetScore(NewScore);
	ScoreChanged.Broadcast(PointDelta, NewScore);

	return NewScore;
}

int32 AVTPlayerState::GetTotalCorrectCount()
{
	return GetTotal(CorrectCounts);
}

int32 AVTPlayerState::GetTotalIncorrectCount()
{
	return GetTotal(IncorrectCounts);
}

int32 AVTPlayerState::GetTotal(const TMap<UPhoneticPhrase*, int32>& Map)
{
	TArray<int32> Counts;
	Map.GenerateValueArray(Counts);

	int Total = 0;
	for(int32 Count : Counts)
	{
		Total += Count;
	}

	return Total;
}

int32 AVTPlayerState::GetCorrectCountByPhrase(UPhoneticPhrase* Phrase)
{
	return CorrectCounts.FindOrAdd(Phrase);
}

int32 AVTPlayerState::GetIncorrectCountByPhrase(UPhoneticPhrase* Phrase)
{
	return CorrectCounts.FindOrAdd(Phrase);
}

int32 AVTPlayerState::GetCorrectCountByText(FString WrittenText)
{
	return GetCounts(WrittenText, CorrectCounts);
}

int32 AVTPlayerState::GetIncorrectCountByText(FString WrittenText)
{
	return GetCounts(WrittenText, IncorrectCounts);
}

int32 AVTPlayerState::GetCounts(FString WrittenText, const TMap<UPhoneticPhrase*, int32>& Map)
{
	TArray<UPhoneticPhrase*> Phrases;
	Map.GenerateKeyArray(Phrases);

	int32 Count = 0;
	for(UPhoneticPhrase* Phrase : Phrases)
	{
		if(WrittenText == Phrase->WrittenText)
		{
			Count += *Map.Find(Phrase);
		}
	}

	return Count;
}