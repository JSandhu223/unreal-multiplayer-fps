#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Weapon.generated.h"


UENUM(BlueprintType)
enum class EFireType : uint8
{
	FullAuto UMETA(DisplayName="Fully Automatic"),
	SemiAuto UMETA(DisplayName="Semi Automatic")
};

UCLASS()
class FPS_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	
	virtual void OnRep_Instigator() override;
	
	USkeletalMeshComponent* GetMesh1P() const;
	
	USkeletalMeshComponent* GetMesh3P() const;
	
	void AttachToOwningPawn() const;
	
	void WeaponTrace(FHitResult& OutHit, float TraceLength);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FPS|Weapon Type")
	FGameplayTag WeaponType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS|Fire Type")
	EFireType FireType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS|Fire Type")
	float FireTime;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FPS|Aiming")
	float AimFieldOfView;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FPS|Trace")
	float TraceRadius;
	
	void Local_Fire(const FVector& ImpactPoint, const FVector& ImpactNormal, TEnumAsByte<EPhysicalSurface> ImpactSurfaceType, bool bIsFirstPerson);
	
	UPROPERTY(EditAnywhere, Category="FPS|Ammo")
	int32 MagCapacity;
	
	UPROPERTY(EditAnywhere, Category="FPS|Ammo")
	int32 Ammo;
	
	UPROPERTY(EditAnywhere, Category="FPS|Ammo")
	int32 StartingCarriedAmmo;
	
	void AuthFire();
	void Rep_Fire(int32 AuthAmmo);

protected:
	// Weapon Mesh: first person view
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FPS|Weapon")
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	// Weapon Mesh: third person view
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FPS|Weapon")
	TObjectPtr<USkeletalMeshComponent> Mesh3P;
	
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void FireEffects(const FVector& ImpactPoint, const FVector& ImpactNormal, EPhysicalSurface ImpactSurfaceType, bool bIsFirstPerson);
	
private:
	void SetMeshVisibilities(APawn* OwningPawn) const;
	
	// Used for client-side prediction of ammo
	int32 Sequence;
};
