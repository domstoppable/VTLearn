#include "PhoneticPhraseFactory.h"

#include "PhoneticPhrase.h"
#include "Subsystems/ImportSubsystem.h"
#include "AssetImportTask.h"

UPhoneticPhraseFactory::UPhoneticPhraseFactory()
	: Super()
{
	bCreateNew = false;
	bEditorImport = true;
	SupportedClass = UPhoneticPhrase::StaticClass();

	Formats.Add(TEXT("vtt;Vibey Transcribe Transcription"));
}

UObject* UPhoneticPhraseFactory::FactoryCreateBinary
(
	UClass*				Class,
	UObject*			InParent,
	FName				Name,
	EObjectFlags		Flags,
	UObject*			Context,
	const TCHAR*		Type,
	const uint8*&		Buffer,
	const uint8*		BufferEnd,
	FFeedbackContext*	Warn
)
{
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, Class, InParent, *Name.ToString(), Type);

	UPhoneticPhrase* ExistingPhrase = FindObject<UPhoneticPhrase>(InParent, *Name.ToString());
	if(!ExistingPhrase)
	{
		ExistingPhrase = NewObject<UPhoneticPhrase>(InParent, Name, Flags);
	}

	if (AssetImportTask && AssetImportTask->bAutomated)
	{
		if (ExistingPhrase)
		{
			if (!AssetImportTask->bReplaceExisting)
			{
				GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, nullptr);
				return nullptr;
			}
		}
	}

	UPhoneticPhrase* Phrase = LoadPhoneticPhrase(Buffer, BufferEnd, ExistingPhrase);
	if(!Phrase)
	{
		UE_LOG(LogTemp, Warning, TEXT("PhoneticPhrase import failed %s"), *Name.ToString());
		GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport( this, nullptr );
		return nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("PhoneticPhrase import ok :) %s"), *Name.ToString());
		UE_LOG(LogTemp, Log, TEXT("\t%s"), *Phrase->WrittenText);
		UE_LOG(LogTemp, Log, TEXT("\t%s"), *Phrase->PhoneticText);
	}

	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, Phrase);

	return Phrase;
}

UObject* UPhoneticPhraseFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool & bOutOperationCanceled)
{
	UObject* Object = Super::FactoryCreateFile(InClass, InParent, InName, Flags, Filename, Parms, Warn, bOutOperationCanceled);
	if(UPhoneticPhrase* Phrase = Cast<UPhoneticPhrase>(Object))
	{
		Phrase->Source = Filename;
	}

	return Object;
}

UPhoneticPhrase* UPhoneticPhraseFactory::LoadPhoneticPhrase(const uint8*& Buffer, const uint8* BufferEnd, UPhoneticPhrase* ExistingPhrase)
{
	UE_LOG(LogTemp, Log, TEXT("Loading VTT from buffer"));
	TArray<uint8> FileData;
	FileData.Empty(BufferEnd - Buffer);
	FileData.AddUninitialized(BufferEnd - Buffer);
	FMemory::Memcpy(FileData.GetData(), Buffer, FileData.Num());

	if(FileData.Num() < 33)
	{
		UE_LOG(LogTemp, Log, TEXT("Bad VTT file size %d"), FileData.Num());
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

	char* WrittenTextChars = new char[TextBytes+1];
	FMemory::Memcpy(WrittenTextChars, FileData.GetData() + Idx, TextBytes);
	WrittenTextChars[TextBytes] = '\0';
	Idx += TextBytes;

	FString WrittenText(WrittenTextChars);

	char* PhoneticTextChars = new char[PhoneticBytes+1];
	FMemory::Memcpy(PhoneticTextChars, FileData.GetData() + Idx, PhoneticBytes);
	PhoneticTextChars[PhoneticBytes] = '\0';
	Idx += PhoneticBytes;

	FString PhoneticText(PhoneticTextChars);
	// strip silence from phonetic text
	PhoneticText = PhoneticText.Replace(TEXT("sp"), TEXT(""));
	PhoneticText = PhoneticText.Replace(TEXT("sil"), TEXT(""));
	PhoneticText = PhoneticText.TrimStartAndEnd();

	uint8* RawSamples = new uint8[SampleCount*3];
	FMemory::Memcpy(RawSamples, FileData.GetData() + Idx, SampleCount*3);

	UPhoneticPhrase* Phrase = ExistingPhrase ? ExistingPhrase : NewObject<UPhoneticPhrase>();
	Phrase->WrittenText = WrittenText;
	Phrase->PhoneticText = PhoneticText;
	Phrase->Period = SamplePeriod;

	TArray<uint8> Samples2 = TArray<uint8>(RawSamples, SampleCount*3);
	Phrase->RawSamples = Samples2;

	Phrase->PhoneCounts = CountPhones(PhoneticText);

	delete[] WrittenTextChars;
	delete[] PhoneticTextChars;
	delete[] RawSamples;

	return Phrase;
}

TMap<FString, int32> UPhoneticPhraseFactory::CountPhones(const FString PhoneText)
{
	TMap<FString, int32> PhoneCounts;

	TArray<FString> Tokens;
	PhoneText.ParseIntoArray(Tokens, TEXT(" "), true);

	for(FString Token : Tokens)
	{
		Token = Token.Left(2);

		if(!PhoneCounts.Contains(Token))
		{
			PhoneCounts.Add(Token, 1);
		}
		else
		{
			int32 Count = PhoneCounts[Token];
			PhoneCounts.Add(Token, Count+1);
		}
	}

	return PhoneCounts;
}
