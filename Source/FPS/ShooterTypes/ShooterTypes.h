#pragma once

#include "ShooterTypes.generated.h"

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	Left UMETA(DisplayName="Turning Left"),
	Right UMETA(DisplayName="Turning Right"),
	NotTurning UMETA(DisplayName="Not Turning")
};

USTRUCT(BlueprintType)
struct FReticleParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShapeCutFactor_RoundFired = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleFactor_RoundFired = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RoundFiredInterpSpeed = 20.0f;
};
