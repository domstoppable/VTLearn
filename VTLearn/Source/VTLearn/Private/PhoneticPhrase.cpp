// Fill out your copyright notice in the Description page of Project Settings.

#include "PhoneticPhrase.h"

#include "HAL/UnrealMemory.h"
#include "Misc/FileHelper.h"

UPhoneticPhrase* UPhoneticPhrase::LoadPhoneticPhrase(FString VTTFilename)
{
	UE_LOG(LogTemp, Log, TEXT("Loading VTT: %s"), *VTTFilename);
	TArray<uint8> FileData;
	FFileHelper::LoadFileToArray(FileData, *VTTFilename);
	if(FileData.Num() < 33)
	{
		UE_LOG(LogTemp, Log, TEXT("Bad VTT file size %d %s"), FileData.Num(), *VTTFilename);
		return nullptr;
	}

	/*
	structFormat = '3s'                         # magic bytes
	structFormat += 'B'                         # file format version
	structFormat += 'I'                         # flags
	structFormat += 'I'                         # size of written text
	structFormat += 'I'                         # size of phonetic text
	structFormat += 'I'                         # sample count
	structFormat += 'I'                         # sample period
	structFormat += str(wordCharCount) + 's'    # written text
	structFormat += str(phoneCharCount) + 's'   # phonetic text
	structFormat += str(sampleByteCount) + 'B'  # samples
	*/

	int32 Idx = 0;
	if(!(FileData[Idx++] == 'V' && FileData[Idx++] == 'T' && FileData[Idx++] == 'T'))
	{
		UE_LOG(LogTemp, Log, TEXT("Bad magic bytes :("));
		return nullptr;
	}

	uint8 FileVersion = FileData[Idx++];

	int32 Flags = 0;
	for(int32 i=0; i<4; i++){
		Flags += int32(FileData[Idx++] << (i*8));
	}

	int32 TextBytes = 0;
	for(int32 i=0; i<4; i++){
		TextBytes += int32(FileData[Idx++] << (i*8));
	}

	int32 PhoneticBytes = 0;
	for(int32 i=0; i<4; i++){
		PhoneticBytes += int32(FileData[Idx++] << (i*8));
	}

	int32 SampleCount = 0;
	for(int32 i=0; i<4; i++){
		SampleCount += int32(FileData[Idx++] << (i*8));
	}

	int32 SamplePeriod = 0;
	for(int32 i=0; i<4; i++){
		SamplePeriod += int32(FileData[Idx++] << (i*8));
	}

	/*
	UE_LOG(LogTemp, Log, TEXT("File version  : %d"), FileVersion);
	UE_LOG(LogTemp, Log, TEXT("Flags         : %d"), Flags);
	UE_LOG(LogTemp, Log, TEXT("TextBytes     : %d"), TextBytes);
	UE_LOG(LogTemp, Log, TEXT("PhoneticBytes : %d"), PhoneticBytes);
	UE_LOG(LogTemp, Log, TEXT("Sample count  : %d"), SampleCount);
	UE_LOG(LogTemp, Log, TEXT("Sample period : %d"), SamplePeriod);
	*/

	char WrittenTextChars[TextBytes+1];
	FMemory::Memcpy(WrittenTextChars, FileData.GetData() + Idx, TextBytes);
	WrittenTextChars[TextBytes] = '\0';
	Idx += TextBytes;

	FString WrittenText(WrittenTextChars);

	char PhoneticTextChars[PhoneticBytes+1];
	FMemory::Memcpy(PhoneticTextChars, FileData.GetData() + Idx, PhoneticBytes);
	PhoneticTextChars[PhoneticBytes] = '\0';
	Idx += PhoneticBytes;

	FString PhoneticText(PhoneticTextChars);
	// strip silence from phonetic text
	PhoneticText = PhoneticText.Replace(TEXT("sp"), TEXT(""));
	PhoneticText = PhoneticText.Replace(TEXT("sil"), TEXT(""));
	PhoneticText = PhoneticText.TrimStartAndEnd();

	uint8 RawSamples[SampleCount*3];
	FMemory::Memcpy(RawSamples, FileData.GetData() + Idx, SampleCount*3);

	UPhoneticPhrase* Phrase = NewObject<UPhoneticPhrase>();
	Phrase->WrittenText = WrittenText;
	Phrase->PhoneticText = PhoneticText;
	Phrase->Period = SamplePeriod;

	TArray<uint8> Samples2 = TArray<uint8>(RawSamples, SampleCount*3);
	Phrase->RawSamples = Samples2;

	// Split up phonetic text
	Phrase->Phonemes = StringToSequence(PhoneticText);

	return Phrase;
}

TArray<UPhoneticPhrase*> UPhoneticPhrase::LoadPhrasesFromDirectory(FString Path)
{
	TArray<UPhoneticPhrase*> Phrases;
	TArray<FString> Files;
	IFileManager& FileManager = IFileManager::Get();

	FileManager.FindFiles(Files, *(Path + "/*.vtt"), true, false);
	for(FString File : Files)
	{
		UPhoneticPhrase* Phrase = LoadPhoneticPhrase(Path + "/" + File);
		if(IsValid(Phrase))
		{
			Phrases.Emplace(Phrase);
		}
	}

	return Phrases;
}

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

TArray<UPhoneticPhrase*> UPhoneticPhrase::LoadPhrases(FString PhraseName)
{
	UE_LOG(LogTemp, Log, TEXT("%s - Loading phrase"), ANSI_TO_TCHAR(__FUNCTION__), *PhraseName);
	TArray<UPhoneticPhrase*> Phrases;
	TArray<FString> Files;
	IFileManager& FileManager = IFileManager::Get();

	FString Path = "/home/dom/Documents/Dissertation Stimuli/vtt/";

	FileManager.FindFiles(Files, *(Path + ("*-" + PhraseName + ".vtt")), true, false);
	FileManager.FindFiles(Files, *(Path + (PhraseName + ".vtt")), true, false);

	for(FString File : Files)
	{
		UPhoneticPhrase* Phrase = LoadPhoneticPhrase(Path + "/" + File);
		if(IsValid(Phrase))
		{
			Phrases.Emplace(Phrase);
		}
	}

	return Phrases;
}
