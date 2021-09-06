// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#include "VTReceiverGrid.h"

#include "Math/UnrealMathUtility.h"

// Sets default values
AVTReceiverGrid::AVTReceiverGrid()
{
	UE_LOG(LogTemp, Log, TEXT("Receiver grid construct"));
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	AreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaBox"));
}

void AVTReceiverGrid::PostInitializeComponents()
{
	if(IsValid(ReceiverClass))
	{
		FVector BoxExtent = AreaBox->GetScaledBoxExtent() * 2.0f;
		FVector BoxLocation = AreaBox->GetComponentLocation();
		FRotator BoxRotation = AreaBox->GetComponentRotation();

		FVector ActorScale = GetActorScale();
		FVector InverseScale(
			1.0f / ActorScale.X,
			1.0f / ActorScale.Y,
			1.0f / ActorScale.Z
		);

		float xSpacing = BoxExtent.X / (XCount+1);
		float ySpacing = BoxExtent.Y / (YCount+1);

		FAttachmentTransformRules AttachRules(EAttachmentRule::KeepWorld, true);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.bAllowDuringConstructionScript = true;
		SpawnParams.Owner = this;

		for(int xIDX=0; xIDX<XCount; xIDX++)
		{
			GridWorldLocations.Emplace(TArray<FVector>());
			for(int yIDX=0; yIDX<YCount; yIDX++)
			{
				FVector Location(
					(BoxLocation.X - BoxExtent.X/2.0f) + ((xIDX+1) * xSpacing),
					(BoxLocation.Y - BoxExtent.Y/2.0f) + ((yIDX+1) * ySpacing),
					BoxLocation.Z
				);
				GridWorldLocations[xIDX].Emplace(Location);
				AVibeyItemReceiver* Receiver = GetWorld()->SpawnActor<AVibeyItemReceiver>(ReceiverClass, Location, BoxRotation, SpawnParams);
				Receiver->AttachToActor(this, AttachRules);
			}
		}
	}

	Super::PostInitializeComponents();
}

void AVTReceiverGrid::BeginPlay()
{
	Super::BeginPlay();
}

TArray<FVector> AVTReceiverGrid::GetOpenLocations()
{
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	TArray<FVector> OpenLocations;

	for(int xIDX=0; xIDX<XCount; xIDX++)
	{
		for(int yIDX=0; yIDX<YCount; yIDX++)
		{
			FVector Location = GridWorldLocations[xIDX][yIDX];

			bool bCellOpen = true;
			for(AActor* Child : AttachedActors)
			{
				FVector ChildLocation = Child->GetActorLocation();

				bool XMatch = FMath::IsNearlyEqual(ChildLocation.X, Location.X, 1.0f);
				bool YMatch = FMath::IsNearlyEqual(ChildLocation.Y, Location.Y, 1.0f);
				if(XMatch && YMatch)
				{
					bCellOpen = false;
					break;
				}
			}
			if(bCellOpen)
			{
				OpenLocations.Add(Location);
			}
		}
	}

	return OpenLocations;
}

void AVTReceiverGrid::MoveToRandomCell(AVibeyItemReceiver* Receiver)
{
	TArray<FVector> OpenLocations = GetOpenLocations();
	FVector Location = OpenLocations[FMath::RandRange(0, OpenLocations.Num()-1)];
	Receiver->SetActorLocation(FVector(Location.X, Location.Y, Receiver->GetActorLocation().Z));
}
