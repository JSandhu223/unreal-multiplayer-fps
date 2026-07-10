#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Weapon.generated.h"


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

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category="FPS|Weapon Type")
	FGameplayTag WeaponType;
	
private:
	// Weapon Mesh: first person view
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	// Weapon Mesh: third person view
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh3P;
	
	void SetMeshVisibilities(APawn* OwningPawn) const;
};
