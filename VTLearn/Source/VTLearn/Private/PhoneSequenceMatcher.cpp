#include "PhoneSequenceMatcher.h"

bool UPhoneSequenceMatcher::Match(UPhoneticPhrase* Phrase)
{
	if(!IsValid(Phrase))
	{
		UE_LOG(LogTemp, Warning, TEXT("UPhoneSequenceMatcher::Match received invalid phrase"));
		return false;
	}

	for(FString SearchString : SearchStrings)
	{
		UE_LOG(LogTemp, Log, TEXT("Checking for '%s' vs '%s'"), *SearchString, *Phrase->PhoneticText);
		if(Phrase->PhoneticText.Equals(SearchString))
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
