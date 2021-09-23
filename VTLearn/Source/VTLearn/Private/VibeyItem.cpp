// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VibeyItem.h"
#include "VTSearchGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVibeyItem::AVibeyItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVibeyItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AVibeyItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVibeyItem::SetPhrase_Implementation(UPhoneticPhrase* NewPhrase)
{
	Phrase = NewPhrase;
	if(IsValid(NewPhrase))
	{
		UE_LOG(LogTemp, Log, TEXT("VibeyItem Phrase has been set to %s"), *NewPhrase->WrittenText);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("VibeyItem Phrase has been set to something invalid?"));
	}
}

void AVibeyItem::StartExpirationTimer(float Period)
{
	GetWorld()->GetTimerManager().SetTimer(
		ExpirationTimerHandle,
		this, &AVibeyItem::ExpireNow,  // Callback
		Period,                        // Rate
		false                          // loop
	);
}

void AVibeyItem::OnAttempted_Implementation()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController->GetPlayerState<AVTPlayerState>()->OnItemAttempted(Phrase, bIsGood, bIsExpired);
	GetWorld()->GetTimerManager().ClearTimer(ExpirationTimerHandle);
}

void AVibeyItem::ExpireNow()
{
	UE_LOG(LogTemp, Log, TEXT("VibeyItem expired %s"), *GetName());

	bIsExpired = true;
	AVTSearchGameMode* SearchGameMode = GetWorld()->GetAuthGameMode<AVTSearchGameMode>();
	if(IsValid(SearchGameMode) && IsValid(SearchGameMode->Matcher) && IsValid(Phrase))
	{
		bIsGood = !SearchGameMode->Matcher->Match(Phrase);
	}

	Expired.Broadcast(this);
	OnExpired();
}

void AVibeyItem::OnExpired_Implementation()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController->GetPlayerState<AVTPlayerState>()->OnItemAttempted(Phrase, bIsGood, bIsExpired);
}

void AVibeyItem::MarkAttempted(bool bNewIsGood)
{
	bIsGood = bNewIsGood;
	Attempted.Broadcast(this);
	OnAttempted();
}