// Fill out your copyright notice in the Description page of Project Settings.


#include "PhonePhraseMatcher.h"

bool UPhonePhraseMatcher::Match(UPhoneticPhrase* Phrase)
{
	return false;
}

bool UPhoneSequenceMatcher::Match(UPhoneticPhrase* Phrase)
{
	return IsValid(Phrase) && Phrase->PhoneticText.Contains(SearchString);
}