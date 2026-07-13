#include "ShooterCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Combat/CombatComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/WeaponData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Weapon/Weapon.h"


AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	// GetCharacterMovement()->MovementState.bCanCrouch = true;
	
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Combat->SetIsReplicated(true);
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 0.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 15.0f;
	SpringArm->bUsePawnControlRotation = true;
	
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(SpringArm);
	FirstPersonCamera->bUsePawnControlRotation = false;
	
	// Note: the camera's field of view is 90.0f by default
	DefaultFieldOfView = 90.0f;
	
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	Mesh1P->SetupAttachment(FirstPersonCamera);
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = false;
	// Only lets the player see their own first-person-based animations (i.e. reloading, ADS)
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	// Ensure all updates for the mesh happen before physics
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	
	// Default character mesh
	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	FirstPersonCamera->SetFieldOfView(DefaultFieldOfView);
}

void AShooterCharacter::BeginDestroy()
{
	Super::BeginDestroy();
	
	if (IsValid(Combat))
	{
		Combat->DestroyInventory();
	}
}

void AShooterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	check(Combat);
	
	Combat->SpawnInventory();
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CalculateTurnInPlaceParameters();
	CalculateFABRIKSocketTransform();
}

void AShooterCharacter::CalculateTurnInPlaceParameters()
{
	// Get velocity, check if it's zero
	// Check if we are falling
	
	// If standing still and not jumping
		// Get current aim rotation
		// Get delta aim rotation (the difference in rotation of current aim rotation from initial aim rotation)
		// (initial aim rotation is calculated in BeginPlay)
		// Store yaw of delta aim rotation in a variable called AO_Yaw
		// If TurningStatus == NotTurning
			// Set InterpAO_Yaw = AO_Yaw
		// TurnInPlace() - interpolates the InterpAO_Yaw value to zero
	
	// If running or jumping
		// Reset initial aim rotation to current aim rotation
		// Set AO_Yaw = 0
		// We also need a Movement Offset Yaw to feed to our strafing blendspaces
		// Get base aim rotation
		// Get movement rotation (this is the rotation of our velocity)
		// Movement Offset Yaw = delta between movement rotation and aim rotation
		// Set TurningStatus = NotTurning
}

// TurnInPlace
	// If AO_Yaw > 90
		// Set TurningStatus = Right
	// Else if AO_Yaw < -90
		// Set TurningStatus = Left
	// if TurningStatus != NotTurning (i.e. we are turning either left or right)
		// Interpolate InterpAO_Yaw down to zero
		// Set AO_Yaw = InterpAO_Yaw
		// if Abs(AO_Yaw) < 5
			// Set TurningStatus = NotTurning
			// Reset initial aim rotation to current aim rotation

void AShooterCharacter::CalculateFABRIKSocketTransform()
{
	if (IsValid(Combat) && IsValid(Combat->CurrentWeapon) && IsValid(Combat->CurrentWeapon->GetMesh3P()))
	{
		// Get the socket named "FABRIK_Socket" from the equipped weapon.
		// Note: every weapon skeletal mesh will have this socket. If using your own meshes, be sure to add socket with this name on the skeletal mesh.
		FABRIK_SocketTransform = Combat->CurrentWeapon->GetMesh3P()->GetSocketTransform("FABRIK_Socket", ERelativeTransformSpace::RTS_World);
		
		// Convert the FABRIK_Socket transform into "hand_r" bone space
		FVector OutLocation;
		FRotator OutRotation;
		GetMesh()->TransformToBoneSpace(
			FName("hand_r"),
			FABRIK_SocketTransform.GetLocation(),
			FABRIK_SocketTransform.GetRotation().Rotator(),
			OutLocation,
			OutRotation
		);
		FABRIK_SocketTransform.SetLocation(OutLocation);
		FABRIK_SocketTransform.SetRotation(OutRotation.Quaternion());
	}
}

FName AShooterCharacter::GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const
{
	checkf(Combat->WeaponData, TEXT("WeaponData not assigned on the CombatComponent of BP_ShooterCharacter"));
	
	return Combat->WeaponData->GripPoints.FindChecked(WeaponType);
}

USkeletalMeshComponent* AShooterCharacter::GetMesh1P_Implementation() const
{
	return Mesh1P;
}

USkeletalMeshComponent* AShooterCharacter::GetMesh3P_Implementation() const
{
	return GetMesh();
}

FRotator AShooterCharacter::GetFixedAimRotation() const
{
	FRotator AimRotation = GetBaseAimRotation();
	
	if (AimRotation.Pitch > 90.0f && !IsLocallyControlled())
	{
		// Map the pitch from [270, 360) to [-90, 0]
		FVector2D InRange(270.0f, 360.0f);
		FVector2D OutRange(-90.0f, 0);
		
		AimRotation.Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimRotation.Pitch);
	}
	
	return AimRotation;
}

bool AShooterCharacter::HasCurrentWeapon() const
{
	return IsValid(Combat) && Combat->CurrentWeapon != nullptr;
}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* ShooterInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	
	ShooterInputComponent->BindAction(CycleWeaponAction, ETriggerEvent::Started, this, &ThisClass::Input_CycleWeapon);
	ShooterInputComponent->BindAction(ReloadWeaponAction, ETriggerEvent::Started, this, &ThisClass::Input_ReloadWeapon);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &ThisClass::Input_FireWeapon_Pressed);
	ShooterInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &ThisClass::Input_FireWeapon_Released);
	ShooterInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Started, this, &ThisClass::Input_AimWeapon_Pressed);
	ShooterInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Completed, this, &ThisClass::Input_AimWeapon_Released);
}

void AShooterCharacter::Input_CycleWeapon()
{
	Combat->Initiate_CycleWeapon();
}

void AShooterCharacter::Input_ReloadWeapon()
{
	Combat->Initiate_ReloadWeapon();
}

void AShooterCharacter::Input_FireWeapon_Pressed()
{
	Combat->Initiate_FireWeapon_Pressed();
}

void AShooterCharacter::Input_FireWeapon_Released()
{
	Combat->Initiate_FireWeapon_Released();
}

void AShooterCharacter::Input_AimWeapon_Pressed()
{
	Combat->Initiate_Aim_Pressed();
	OnAim(true);
}

void AShooterCharacter::Input_AimWeapon_Released()
{
	Combat->Initiate_Aim_Released();
	OnAim(false);
}
