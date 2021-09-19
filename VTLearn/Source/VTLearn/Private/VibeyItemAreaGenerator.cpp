// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VibeyItemAreaGenerator.h"

#include "VTPlayerState.h"
#include "VTSearchGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AVibeyItemAreaGenerator::AVibeyItemAreaGenerator()
{
	AreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaBox"));
	AreaBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AVibeyItemAreaGenerator::AreaBeginOverlap);
	AreaBox->OnComponentEndOverlap.AddUniqueDynamic(this, &AVibeyItemAreaGenerator::AreaEndOverlap);
}

// Called when the game starts or when spawned
void AVibeyItemAreaGenerator::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnTimer();
	GetWorld()->GetTimerManager().PauseTimer(SpawnDelayTimerHandle);
}

void AVibeyItemAreaGenerator::SpawnItem()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	APawn* PlayerPawn = PlayerController->GetPawn();

	if(PlayerPawn->GetVelocity().Size2D() < 0.1f)
	{
		UE_LOG(LogTemp, Log, TEXT("AVibeyItemAreaGenerator not spawning because player is not moving. Will try again in %0.2f"), TriggerRetryTime);
		GetWorld()->GetTimerManager().SetTimer(
			SpawnDelayTimerHandle,
			this, &AVibeyItemAreaGenerator::SpawnItem,	// Callback
			TriggerRetryTime,							// Rate
			false                                       // loop
		);
	}
	else
	{
		// Spawn item
		UPhoneticPhrase* Phrase = RandomPhrase();
		if(IsValid(Phrase))
		{
			FVector Location = PlayerPawn->GetActorLocation();
			FRotator Rotation = PlayerPawn->GetActorRotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			CurrentItem = GetWorld()->SpawnActor<AVibeyItem>(ItemClass, FVector(-500, -500, -500), Rotation, SpawnParams);
			CurrentItem->SetPhrase(Phrase);

			CurrentItem->SetActorLocation(Location, false, nullptr, ETeleportType::ResetPhysics);

			CurrentItem->GetStaticMeshComponent()->SetCollisionProfileName(FName(TEXT("OverlapAll")), true);
			CurrentItem->GetStaticMeshComponent()->SetSimulatePhysics(false);
			CurrentItem->GetStaticMeshComponent()->SetHiddenInGame(true);

			GetWorld()->GetTimerManager().SetTimer(
				KillDelayTimerHandle,
				this, &AVibeyItemAreaGenerator::KillItem,   // Callback
				ItemTimeToLive,								// Rate
				false                                       // loop
			);

			UE_LOG(LogTemp, Log, TEXT("Spawned new item: %s"), *Phrase->WrittenText);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to generate phrase"));
		}
	}
}

void AVibeyItemAreaGenerator::StartSpawnTimer()
{
	GetWorld()->GetTimerManager().SetTimer(
		SpawnDelayTimerHandle,
		this, &AVibeyItemAreaGenerator::SpawnItem,          // Callback
		FMath::RandRange(MinTriggerTime, MaxTriggerTime),   // Rate
		false                                               // loop
	);
}

void AVibeyItemAreaGenerator::KillItem()
{
	if (IsValid(CurrentItem) && CurrentItem->bGrabbable)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

		AVTSearchGameMode* GameMode = GetWorld()->GetAuthGameMode<AVTSearchGameMode>();
		CurrentItem->bIsGood = !GameMode->Matcher->Match(CurrentItem->Phrase);

		AVTPlayerState* PlayerState = PlayerController->GetPlayerState<AVTPlayerState>();
		PlayerState->OnItemAttempted(CurrentItem->Phrase, CurrentItem->bIsGood);
		CurrentItem->Expired();
	}
	CurrentItem = nullptr;

	StartSpawnTimer();
}

void AVibeyItemAreaGenerator::AreaBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult )
{
	if(IsPlayerInteractionComponent(Other, OtherComp))
	{
		UE_LOG(LogTemp, Log, TEXT("Player has entered the AreaGenerator box"));
		GetWorld()->GetTimerManager().UnPauseTimer(SpawnDelayTimerHandle);
	}
}

void AVibeyItemAreaGenerator::AreaEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(IsPlayerInteractionComponent(Other, OtherComp))
	{
		UE_LOG(LogTemp, Log, TEXT("Player has left the AreaGenerator box"));
		GetWorld()->GetTimerManager().PauseTimer(SpawnDelayTimerHandle);
	}
}

bool AVibeyItemAreaGenerator::IsPlayerInteractionComponent(AActor* Actor, UPrimitiveComponent* Component)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	APawn* PlayerPawn = PlayerController->GetPawn();

	return (Actor == PlayerPawn) && Component->ComponentHasTag("interaction");
}
