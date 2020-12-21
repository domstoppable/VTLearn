// Fill out your copyright notice in the Description page of Project Settings.


#include "PhonePhraseMatcher.h"

bool UPhonePhraseMatcher::Match(UPhoneticPhrase* Phrase)
{
	return false;
}

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
