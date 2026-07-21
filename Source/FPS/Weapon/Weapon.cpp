#include "Weapon.h"

#include "FPS.h"
#include "KismetTraceUtils.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/PlayerInterface.h"
#include "Kismet/KismetMathLibrary.h"


TAutoConsoleVariable<bool> CVarWeaponTraceDebugDrawing(
	TEXT("game.weapon.trace.DebugDraw"), 
	false, 
	TEXT("Enable debug drawing for tracing weapon fire. (0 = disable, 1 = enable)"),
	ECVF_Cheat
);


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
	
	FireTime = 0.1f;
	
	AimFieldOfView = 65.0f;
	
	TraceRadius = 5.0f;
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

void AWeapon::WeaponTrace(FHitResult& OutHit, float TraceLength)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(GetOwner());
	
	FCollisionResponseParams ResponseParams;
	// Ignore all channels. Then pick which specific channels to block.
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldStatic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldDynamic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_PhysicsBody, ECR_Block);
	
	ensure(GetInstigator());
	APlayerController* PC = CastChecked<APlayerController>(GetInstigator()->GetController());
	if (IsValid(PC))
	{
		FVector EyesWorldLocation;
		FRotator EyesWorldRotation;
		PC->GetActorEyesViewPoint(EyesWorldLocation, EyesWorldRotation);
		
		const FVector EyesWorldDirection = UKismetMathLibrary::GetForwardVector(EyesWorldRotation);
		
		const FVector Start = EyesWorldLocation;
		const FVector End = Start + (EyesWorldDirection * TraceLength);
		
		const bool bHit = GetWorld()->SweepSingleByChannel(
			OutHit,
			Start,
			End,
			FQuat::Identity,
			FPSTraceChannel::ECC_Weapon,
			FCollisionShape::MakeSphere(TraceRadius),
			QueryParams,
			ResponseParams
		);
		
		// If the trace doesn't hit, then manually set the impact point.
		if (!bHit)
		{
			OutHit.ImpactPoint = End;
		}
		
		bool bEnabledDebugDraw = CVarWeaponTraceDebugDrawing.GetValueOnGameThread();
		if (bEnabledDebugDraw)
		{
			DrawDebugSphereTraceSingle(
				GetWorld(),
				Start,
				End,
				TraceRadius,
				EDrawDebugTrace::ForDuration,
				bHit,
				OutHit,
				FColor::Red,
				FColor::Green,
				5.0f
			);
		}
	}
}

void AWeapon::Local_Fire(const FVector& ImpactPoint, const FVector& ImpactNormal,
	TEnumAsByte<EPhysicalSurface> ImpactSurfaceType, bool bIsFirstPerson)
{
	bool bEnabledDebugDraw = CVarWeaponTraceDebugDrawing.GetValueOnGameThread();
	if (bEnabledDebugDraw)
	{
		DrawDebugSphere(GetWorld(), ImpactPoint, 5.0f, 12, FColor::White, false, 3.0f);
	}
	
	FireEffects(ImpactPoint, ImpactNormal, ImpactSurfaceType, bIsFirstPerson);
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
