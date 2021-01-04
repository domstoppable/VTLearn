// Fill out your copyright notice in the Description page of Project Settings.


#include "VTPlayerState.h"

#include "VTLearnGameMode.h"

int32 AVTPlayerState::OnItemAttempted(UPhoneticPhrase* Phrase, bool bCorrect)
{
	// Store attempt
	FPhrasePerformance Performance = Counts.FindOrAdd(Phrase);
	if(bCorrect)
	{
		Performance.Correct++;
	}
	else
	{
		Performance.Incorrect++;
	}
	Counts.Add(Phrase, Performance);

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
	TArray<FPhrasePerformance> AllCounts;
	Counts.GenerateValueArray(AllCounts);

	int Total = 0;
	for(FPhrasePerformance Count : AllCounts)
	{
		Total += Count.Correct;
	}

	return Total;
}

int32 AVTPlayerState::GetTotalIncorrectCount()
{
	TArray<FPhrasePerformance> AllCounts;
	Counts.GenerateValueArray(AllCounts);

	int Total = 0;
	for(FPhrasePerformance Count : AllCounts)
	{
		Total += Count.Incorrect;
	}

	return Total;
}

TMap<FString, FPhrasePerformance> AVTPlayerState::ReportByText()
{
	TMap<FString, FPhrasePerformance> Report;

	TArray<UPhoneticPhrase*> Phrases;
	Counts.GenerateKeyArray(Phrases);

	for(UPhoneticPhrase* Phrase : Phrases)
	{
		FPhrasePerformance ReportCount = Report.FindOrAdd(Phrase->WrittenText);

		FPhrasePerformance Count = Counts.FindOrAdd(Phrase);
		UE_LOG(LogTemp, Log, TEXT("Reporting on %s, %d, %d"), *Phrase->WrittenText, Count.Correct, Count.Incorrect);
		ReportCount.Correct += Count.Correct;
		ReportCount.Incorrect += Count.Incorrect;

		Report.Add(Phrase->WrittenText, ReportCount);
	}

	return Report;
}
