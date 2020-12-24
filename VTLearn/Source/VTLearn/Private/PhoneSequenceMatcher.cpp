#include "PhoneSequenceMatcher.h"

bool UPhoneSequenceMatcher::Match(UPhoneticPhrase* Phrase)
{
	if(!IsValid(Phrase))
	{
		return false;
	}

	for(FString SearchString : SearchStrings)
	{
		UE_LOG(LogTemp, Log, TEXT("Checking for '%s' in '%s'"), *SearchString, *Phrase->PhoneticText);
		if(Phrase->PhoneticText.Contains(SearchString))
		{
			return true;
		}
	}

	return false;
}

void UPhoneSequenceMatcher::SetMatchPhrases(TArray<UPhoneticPhrase*> Phrases)
{
	UE_LOG(LogTemp, Log, TEXT("Going to match %d phrases"), Phrases.Num());
	SearchStrings.Empty();

	for(UPhoneticPhrase* Phrase : Phrases)
	{
		UE_LOG(LogTemp, Log, TEXT("Matching %s"), *Phrase->PhoneticText);
		SearchStrings.Emplace(Phrase->PhoneticText);
	}
}
