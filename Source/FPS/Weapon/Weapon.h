#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "ShooterTypes/ShooterTypes.h"
#include "Weapon.generated.h"


UENUM(BlueprintType)
enum class EFireType : uint8
{
	FullAuto UMETA(DisplayName="Fully Automatic"),
	SemiAuto UMETA(DisplayName="Semi Automatic")
};

UENUM()
enum EWeaponStatus : uint8
{
	Idle, /* (or Equipped). Weapon doing nothing, can fire/reload/cycle */
	Firing, /* Currently firing, cannot reload/cycle */
	Reloading, /* Currently reloading, cannot fire/cycle */
	Cycling, /* Currently cycling to next weapon, cannot fire/reload/cycle */
	Unequipped /* Weapon exists in player inventory but is not the CurrentWeapon, cannot perform any action on it */
};


UCLASS()
class FPS_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	
	USkeletalMeshComponent* GetMesh1P() const;
	USkeletalMeshComponent* GetMesh3P() const;
	
	UMaterialInstanceDynamic* GetReticleDynamicMaterialInstance();
	UMaterialInstanceDynamic* GetAmmoCounterDynamicMaterialInstance();
	
	void AttachToOwningPawn(APawn* Pawn) const;
	
	void WeaponTrace(FHitResult& OutHit, float TraceLength);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FPS|Weapon Type")
	FGameplayTag WeaponType;
	
	EWeaponStatus WeaponStatus;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS|Fire Type")
	EFireType FireType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FPS|Fire Type")
	float FireTime;
	
	UPROPERTY(EditDefaultsOnly, Category="FPS|Reticle")
	FReticleParams ReticleParams;
	
	UPROPERTY(EditDefaultsOnly, Category="FPS|Icon")
	TObjectPtr<UMaterialInterface> WeaponIcon;
	
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
	
	UPROPERTY(EditDefaultsOnly, Category="FPS|Weapon")
	TObjectPtr<UMaterialInterface> ReticleMaterial;
	
	UPROPERTY(EditDefaultsOnly, Category="FPS|Weapon")
	TObjectPtr<UMaterialInterface> AmmoCounterMaterial;
	
	UPROPERTY(VisibleAnywhere, Category="FPS|Weapon")
	TObjectPtr<UMaterialInstanceDynamic> DynMatInst_Reticle;
	
	UPROPERTY(VisibleAnywhere, Category="FPS|Weapon")
	TObjectPtr<UMaterialInstanceDynamic> DynMatInst_AmmoCounter;
};
