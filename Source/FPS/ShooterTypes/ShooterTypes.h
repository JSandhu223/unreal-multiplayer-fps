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
	
	// Shape Cut Factor
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShapeCutFactor_RoundFired = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShapeCutFactor_Aiming = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShapeCutFactor_NotAiming = 0.0f;
	
	// Scale Factor
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleFactor_RoundFired = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleFactor_Aiming = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleFactor_NotAiming = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleFactor_Targeting = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleFactor_NotTargeting = 0.0f;
	
	// Interp Speeds
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RoundFiredInterpSpeed = 20.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AimingInterpSpeed = 15.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetingPlayerInterpSpeed = 10.0f;
};
