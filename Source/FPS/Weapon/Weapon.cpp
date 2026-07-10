#include "Weapon.h"

#include "Components/SkeletalMeshComponent.h"


AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = 0;
	//Mesh1P->SetHiddenInGame(true);
	SetRootComponent(Mesh1P);
	
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh3P"));
	Mesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = 1;
	//Mesh3P->SetHiddenInGame(true);
	Mesh3P->SetupAttachment(Mesh1P);
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
