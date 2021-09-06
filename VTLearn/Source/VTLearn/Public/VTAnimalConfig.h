// (c) 2021 Dominic Canare <dom@dominiccanare.com>

#pragma once

#include "Engine/StaticMesh.h"

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VTAnimalConfig.generated.h"

UCLASS()
class VTLEARN_API UVTAnimalConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* BodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* EarsMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor BaseColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* SkinTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* FaceTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* ArmTexture;

};
