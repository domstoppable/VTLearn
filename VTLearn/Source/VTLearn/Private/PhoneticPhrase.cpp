// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "PhoneticPhrase.h"

#include "HAL/UnrealMemory.h"
#include "Misc/FileHelper.h"

TArray<UPhoneticPhrase*> UPhoneticPhrase::LoadPhrases(FString PhraseName)
{
	TArray<FString> Prefixes;
	Prefixes.Add(TEXT("f1"));
	Prefixes.Add(TEXT("f2"));
	Prefixes.Add(TEXT("m1"));
	Prefixes.Add(TEXT("m2"));

	TArray<UPhoneticPhrase*> Phrases;
	for(FString Prefix : Prefixes)
	{
		FString ReferencePath = FString::Printf(TEXT("/Game/Data/VTT/%s-%s"), *Prefix, *PhraseName);
		UPhoneticPhrase* Phrase = LoadObject<UPhoneticPhrase>(NULL, *ReferencePath, NULL, LOAD_None, NULL);
		if(IsValid(Phrase))
		{
			Phrases.Emplace(Phrase);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to find phrase `%s`"), *ReferencePath);
		}
	}

	return Phrases;
}