#include "Weapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/PlayerInterface.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = 0;
	Mesh1P->SetHiddenInGame(true);
	SetRootComponent(Mesh1P);
	
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh3P"));
	Mesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = 1;
	Mesh3P->SetHiddenInGame(true);
	Mesh3P->SetupAttachment(Mesh1P);
}

void AWeapon::OnRep_Instigator()
{
	Super::OnRep_Instigator();
	
	// Quick way to ensure weapon attaches on clients
	AttachToOwningPawn();
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

USkeletalMeshComponent* AWeapon::GetMesh1P() const
{
	return Mesh1P;
}

USkeletalMeshComponent* AWeapon::GetMesh3P() const
{
	return Mesh3P;
}

void AWeapon::AttachToOwningPawn() const
{
	APawn* OwningPawn = GetInstigator();
	if (!IsValid(OwningPawn) || !OwningPawn->Implements<UPlayerInterface>()) { return; }
	
	SetMeshVisibilities(OwningPawn);
	
	const FName AttachPoint = IPlayerInterface::Execute_GetWeaponAttachPoint(OwningPawn, WeaponType);
	USkeletalMeshComponent* PawnMesh1P = IPlayerInterface::Execute_GetMesh1P(OwningPawn);
	USkeletalMeshComponent* PawnMesh3P = IPlayerInterface::Execute_GetMesh3P(OwningPawn);
	
	// Note: if the mesh isn't visible, then attachment won't occur
	// We are assuming the AttachPoint has the same name on both the weapon meshes
	Mesh1P->AttachToComponent(PawnMesh1P, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	Mesh3P->AttachToComponent(PawnMesh3P, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
}

void AWeapon::SetMeshVisibilities(APawn* OwningPawn) const
{
	if (OwningPawn->IsLocallyControlled())
	{
		// Hide only the third person weapon mesh
		Mesh1P->SetHiddenInGame(false);
		Mesh3P->SetHiddenInGame(true);
	}
	else
	{
		// Hide only the first person weapon mesh
		Mesh1P->SetHiddenInGame(true);
		Mesh3P->SetHiddenInGame(false);
	}
}
