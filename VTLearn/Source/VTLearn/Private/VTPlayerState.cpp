// Fill out your copyright notice in the Description page of Project Settings.


#include "VTPlayerState.h"

#include "VTLearnGameMode.h"

#include "Data/PsydekickData.h"
#include "Data/CSVLogger.h"
#include "VTGameInstance.h"

#include "Misc/Paths.h"

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

	if(!DataLogger)
	{
		DataLogger = UPsydekickData::CreateCSVLogger(TEXT("TrainingLog"), TEXT("TrainingData"));
		TArray<FString> FieldNames;

		FieldNames.Add(TEXT("PID"));
		FieldNames.Add(TEXT("Level"));
		FieldNames.Add(TEXT("Stimulus"));
		FieldNames.Add(TEXT("Correct"));

		DataLogger->SetFieldNames(FieldNames);
	}

	UVTGameInstance* GameInstance = UVTGameInstance::GetVTGameInstance(this);
	TMap<FString, FString> LogRecord;

	LogRecord.Add("PID", FString::Printf(TEXT("%d"), GameInstance->LoadedSave->PID));
	LogRecord.Add("Level", FString::Printf(TEXT("%s"), *GameInstance->CurrentLevelStatus->LevelConfig.Name));
	LogRecord.Add("Stimulus", FString::Printf(TEXT("%s"), *FPaths::GetCleanFilename(Phrase->Source)));
	LogRecord.Add("Correct", FString::Printf(TEXT("%d"), Performance.Correct));

	DataLogger->LogStrings(LogRecord);

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
