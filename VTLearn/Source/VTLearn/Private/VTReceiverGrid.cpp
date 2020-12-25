// Fill out your copyright notice in the Description page of Project Settings.


#include "VTReceiverGrid.h"

// Sets default values
AVTReceiverGrid::AVTReceiverGrid()
{
	UE_LOG(LogTemp, Log, TEXT("Receiver grid construct"));
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	AreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaBox"));
}

void AVTReceiverGrid::BeginPlay()
{
	Super::BeginPlay();

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
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AVibeyItemReceiver* Receiver = GetWorld()->SpawnActor<AVibeyItemReceiver>(ReceiverClass, Location, BoxRotation, SpawnParams);
			Receiver->AttachToActor(this, AttachRules);

			UE_LOG(LogTemp, Log, TEXT("Spawned receiver at %0.2f, %0.2f, %0.2f"), Location.X, Location.Y, Location.Z);

			//CellAssignments.Add(FIntVector(xIDX, yIDX)) = Receiver;
		}
	}
}

TArray<FVector> AVTReceiverGrid::GetOpenLocations()
{
	TArray<AActor*> Children;
	GetAttachedActors(Children);

	TArray<FVector> OpenLocations;

	for(int xIDX=0; xIDX<XCount; xIDX++)
	{
		for(int yIDX=0; yIDX<YCount; yIDX++)
		{
			FVector Location = GridWorldLocations[xIDX][yIDX];

			bool bCellOpen = true;
			for(AActor* Child : Children)
			{
				FVector ChildLocation = Child->GetActorLocation();
				if(ChildLocation.X == Location.X && ChildLocation.Y == Location.Y)
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
