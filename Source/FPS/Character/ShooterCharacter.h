#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInterface.h"
#include "ShooterCharacter.generated.h"


class UInputAction;
class UCombatComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class FPS_API AShooterCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	
	virtual void BeginPlay() override;
	
	virtual void BeginDestroy() override;
	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	/** PlayerInterface */
	
	virtual FName GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const override;
	
	virtual USkeletalMeshComponent* GetMesh1P_Implementation() const override;
	
	virtual USkeletalMeshComponent* GetMesh3P_Implementation() const override;
	
	/** ~PlayerInterface */
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FPS|Combat")
	TObjectPtr<UCombatComponent> Combat;
	
private:
	// First person arms
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	
	UPROPERTY(EditDefaultsOnly, Category="FPS|Input")
	TObjectPtr<UInputAction> CycleWeaponAction;
	
	UPROPERTY(EditDefaultsOnly, Category="FPS|Input")
	TObjectPtr<UInputAction> ReloadWeaponAction;
	
	UPROPERTY(EditDefaultsOnly, Category="FPS|Input")
	TObjectPtr<UInputAction> FireWeaponAction;
	
	UPROPERTY(EditDefaultsOnly, Category="FPS|Input")
	TObjectPtr<UInputAction> AimWeaponAction;
	
	void Input_CycleWeapon();
	void Input_ReloadWeapon();
	void Input_FireWeapon_Pressed();
	void Input_FireWeapon_Released();
	void Input_AimWeapon_Pressed();
	void Input_AimWeapon_Released();
};
