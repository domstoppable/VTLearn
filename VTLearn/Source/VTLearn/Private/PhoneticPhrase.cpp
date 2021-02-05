// Fill out your copyright notice in the Description page of Project Settings.

#include "PhoneticPhrase.h"

#include "HAL/UnrealMemory.h"
#include "Misc/FileHelper.h"

TArray<EPhoneme> UPhoneticPhrase::StringToSequence(FString PhoneText)
{
	TArray<EPhoneme> Sequence;
	TArray<FString> Tokens;
	PhoneText.ParseIntoArray(Tokens, TEXT(" "), true);

	const UEnum* PhonemeEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPhoneme"), true);
	for(FString Token : Tokens)
	{
		Token = Token.Left(2);
		int32 Index = PhonemeEnum->GetIndexByNameString(Token);
		if(Index == INDEX_NONE)
		{
			UE_LOG(LogTemp, Error, TEXT("Could not find Phoneme value: %s"), *Token);
		}else{
			EPhoneme Phoneme = EPhoneme((uint8)Index);
			Sequence.Emplace(Phoneme);
		}
	}

	return Sequence;
}
