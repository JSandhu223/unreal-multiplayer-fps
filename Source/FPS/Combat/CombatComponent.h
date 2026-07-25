#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "ShooterTypes/ShooterTypes.h"
#include "CombatComponent.generated.h"


class UMaterialInstanceDynamic;
class AWeapon;
class UWeaponData;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReticleChanged, UMaterialInstanceDynamic*, ReticleDynMatInst, const FReticleParams&, ReticleParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAmmoCounterChanged, UMaterialInstanceDynamic*, AmmoCounterDynMatInst, int32, RoundsCurrent, int32, RoundsMax);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRoundFired, int32, RoundsCurrent, int32, RoundsMax);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintPure, Category="FPS|Combat")
	static UCombatComponent* FindCombatComponent(const AActor* Actor);
	
	// Cycle to the next weapon in inventory
	void Initiate_CycleWeapon();
	void Initiate_ReloadWeapon();
	void Initiate_FireWeapon_Pressed();
	void Initiate_FireWeapon_Released();
	void Initiate_Aim_Pressed();
	void Initiate_Aim_Released();
	
	UPROPERTY(BlueprintAssignable)
	FOnReticleChanged OnReticleChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnAmmoCounterChanged OnAmmoCounterChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnRoundFired OnRoundFired;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FPS|Weapon")
	TObjectPtr<UWeaponData> WeaponData;
	
	// Called only on the server
	void Equip(AWeapon* Weapon);
	
	void SpawnInventory();
	void DestroyInventory();
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bAiming;
	
	UPROPERTY(Transient, BlueprintReadOnly, ReplicatedUsing=OnRep_CurrentWeapon)
	TObjectPtr<AWeapon> CurrentWeapon;
	
	void InitializeWeaponWidgets() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="FPS|Weapon")
	float TraceLength;
	
private:
	bool bTriggerPressed;
	FTimerHandle FireTimer;
	void FireTimerFinished();
	
	// Called when CurrentWeapon replicates to clients
	UFUNCTION()
	void OnRep_CurrentWeapon(AWeapon* LastWeapon);
	
	UPROPERTY(Transient, Replicated)
	TArray<AWeapon*> Inventory;
	
	UPROPERTY(EditDefaultsOnly, Category="FPS|Weapon")
	TArray<TSubclassOf<AWeapon>> DefaultWeaponClasses;
	
	AWeapon* SpawnWeapon(TSubclassOf<AWeapon> WeaponClass) const;
	
	// Server RPC for letting server and other clients know when a client is aiming their weapon
	UFUNCTION(Server, Reliable)
	void Server_Aim(bool bPressed);
	
	void Local_Aim(bool bPressed);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FireWeapon(const FHitResult& Hit, int32 AuthAmmo);
	
	UFUNCTION(Server, Reliable)
	void Server_FireWeapon(const FHitResult& Hit);
	
	void Local_FireWeapon();
};
