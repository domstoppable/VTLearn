// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VibeyItemReceiver.h"
#include "VibeyItem.h"

// Sets default values
AVibeyItemReceiver::AVibeyItemReceiver()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AVibeyItemReceiver::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AVibeyItemReceiver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVibeyItemReceiver::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Matcher = NewObject<UPhoneSequenceMatcher>();
}

void AVibeyItemReceiver::ReceiveItem_Implementation(AVibeyItem* Item)
{
	CheckItem(Item);
}

void AVibeyItemReceiver::SetHelpText(FString Text)
{
	HelpText = Text;
	HelpTextChanged(Text);
}

void AVibeyItemReceiver::SetMatchPhrases(TArray<UPhoneticPhrase*> Phrases)
{
	UE_LOG(LogTemp, Log, TEXT("Setting receiver to take %d match phrases"), Phrases.Num());
	if (!Matcher) {
		Matcher = NewObject<UPhoneSequenceMatcher>();
	}

	Matcher->SetMatchPhrases(Phrases);
	if(Phrases.Num() > 0)
	{
		SetHelpText(Phrases[0]->WrittenText);
	}
	else
	{
		SetHelpText(TEXT(""));
	}
}

bool AVibeyItemReceiver::CheckItem(AVibeyItem* Item, bool bMarkAsAttempted)
{
	bool bResult = false;

	if(IsValid(Matcher) && IsValid(Item) && IsValid(Item->Phrase))
	{
		bResult = Matcher->Match(Item->Phrase);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not check item because of bad matcher or bad item"));
	}

	if(bMarkAsAttempted)
	{
		Item->MarkAttempted(bResult);
	}

	return bResult;
}