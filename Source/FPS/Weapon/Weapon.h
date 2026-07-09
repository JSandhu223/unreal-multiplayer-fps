#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"


UCLASS()
class FPS_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	
	USkeletalMeshComponent* GetMesh1P() const;
	
	USkeletalMeshComponent* GetMesh3P() const;

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
};
