// (c) 2021 Dominic Canare <dom@dominiccanare.com>


#include "PhonemeTrainingTracker.h"

#include "LevelConfig.h"

#include "PhoneticPhrase.h"
#include "Engine/DataTable.h"

void UPhonemeTrainingTracker::SetTargetFrequenciesFromCounts(UDataTable* TargetPhonemeCounts)
{
	UE_LOG(LogTemp, Log, TEXT("UPhonemeTrainingTracker::SetTargetFrequenciesFromCounts"));

	float TotalCount = 0.0f;
	FString ContextString;

	TArray<FName> PhonemeRowNames = TargetPhonemeCounts->GetRowNames();
	for(FName RowName : PhonemeRowNames)
	{
		FPhoneCount* PhoneCount = TargetPhonemeCounts->FindRow<FPhoneCount>(RowName, ContextString);
		if(!PhoneCount->TrainPhoneme.Equals(""))
		{
			TotalCount += PhoneCount->PhonemeExposures;
			TargetPhonemeFrequencies.Emplace(PhoneCount->TrainPhoneme, PhoneCount->PhonemeExposures);
		}
	}

	TArray<FString> Phones;
	TargetPhonemeFrequencies.GenerateKeyArray(Phones);
	for (FString Phone : Phones)
	{
		TargetPhonemeFrequencies.Emplace(Phone, TargetPhonemeFrequencies[Phone] / TotalCount);
	}
}

void UPhonemeTrainingTracker::UpdateCurrentPhonemeFrequencies(TMap<FString, int32> PhoneCounts)
{
	UE_LOG(LogTemp, Log, TEXT("UPhonemeTrainingTracker::UpdateCurrentPhonemeFrequencies"));

	float TotalCount = 0.0f;
	for (auto& Elem : PhoneCounts)
	{
		TotalCount += Elem.Value;
		UE_LOG(LogTemp, Log, TEXT("UpdateCurrentPhonemeFrequencies - Adding %s"), *Elem.Key);
		CurrentPhonemeFrequencies.FindOrAdd(Elem.Key, Elem.Value);
		CurrentPhonemeFrequencies.Add(Elem.Key, Elem.Value);
	}

	TArray<FString> Phones;
	CurrentPhonemeFrequencies.GenerateKeyArray(Phones);
	for (FString Phone : Phones)
	{
		CurrentPhonemeFrequencies.Emplace(Phone, CurrentPhonemeFrequencies.FindOrAdd(Phone, 0) / TotalCount);
	}
}

float UPhonemeTrainingTracker::GetMultiplier(FLevelConfig LevelConfig)
{
	float Multiplier = 0.0f;

	UE_LOG(LogTemp, Log, TEXT("Get multiplier for %s"), *(LevelConfig.Name));

	TMap<FString, float> PhoneWeights;

	// get total counts of each phoneme in each vtt that exists for each phrases
	float TotalCount = 0.0f;
	for(FString PhraseName : LevelConfig.TrainingPhrases)
	{
		for(UPhoneticPhrase* Phrase : UPhoneticPhrase::LoadPhrases(PhraseName))
		{
			for(auto& Elem : Phrase->PhoneCounts)
			{
				int32 Value = PhoneWeights.FindOrAdd(Elem.Key, 0);
				PhoneWeights.Add(Elem.Key, Value + Elem.Value);

				TotalCount += Elem.Value;
			}
		}
	}
	// convert those counts to weights
	// compare those weights to expected weights
	for(auto& Elem : PhoneWeights)
	{
		float PhoneWeightInLevel = PhoneWeights[Elem.Key] / TotalCount;
		float Need = TargetPhonemeFrequencies.FindOrAdd(Elem.Key, 1.0f) - CurrentPhonemeFrequencies.FindOrAdd(Elem.Key, 0);

		Multiplier += 100*(PhoneWeightInLevel * Need);
	}

	return Multiplier;
}
