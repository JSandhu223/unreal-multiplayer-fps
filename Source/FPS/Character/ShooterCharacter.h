#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInterface.h"
#include "ShooterTypes/ShooterTypes.h"
#include "ShooterCharacter.generated.h"


class UInputAction;
class UCombatComponent;
class USpringArmComponent;
class UCameraComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFirstReplicated, AWeapon*, Weapon);


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
	
	/** PlayerInterface*/
	virtual FName GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const override;
	virtual USkeletalMeshComponent* GetMesh1P_Implementation() const override;
	virtual USkeletalMeshComponent* GetMesh3P_Implementation() const override;
	virtual void WeaponReplicated_Implementation() override; // Called by combat component
	
	// Fixes the pitch by mapping the range [270, 360] to [-90, 0].
	UFUNCTION(BlueprintCallable)
	FRotator GetFixedAimRotation() const;
	
	UPROPERTY(BlueprintReadOnly, Category="FPS|FABRIK")
	FTransform FABRIK_SocketTransform;
	
	// Used to fix the arm warping around the model in the animation blueprint preview
	UFUNCTION(BlueprintCallable)
	bool HasCurrentWeapon() const;
	
	UPROPERTY(BlueprintAssignable)
	FOnWeaponFirstReplicated OnWeaponFirstReplicated;
	
	bool bWeaponFirstReplicated;
	
	bool HasWeaponFirstReplicated() const { return bWeaponFirstReplicated; }
	
protected:
	// First person arms
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FPS|Camera")
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FPS|Aiming")
	float DefaultFieldOfView;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FPS|Combat")
	TObjectPtr<UCombatComponent> Combat;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnAim(bool bIsAiming);

	
private:
	void CalculateFABRIKSocketTransform();
	
	void CalculateTurnInPlaceParameters(float DeltaTime);
	
	void TurnInPlace(float DeltaTime);
	
protected:
	FRotator StartingAimRotation;
	
	float InterpAO_Yaw = 0.0f;
	
	// This is the yaw of the DeltaRotation
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="FPS|Turn in Place")
	float AO_Yaw = 0.0f;
	
	// This ensures that AO_Yaw isn't reversed
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="FPS|Turn in Place")
	float NegatedAO_Yaw = 0.0f;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="FPS|Turn in Place")
	ETurningInPlace TurningStatus;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="FPS|Strafing")
	float MovementOffsetYaw;
	
private:
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
