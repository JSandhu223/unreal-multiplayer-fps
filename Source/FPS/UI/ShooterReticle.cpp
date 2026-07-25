#include "ShooterReticle.h"

#include "Character/ShooterCharacter.h"
#include "Combat/CombatComponent.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Weapon/Weapon.h"


namespace Ammo
{
	// These match the names in the Material Instance asset
	const FName Rounds_Current = FName("Rounds_Current");
	const FName Rounds_Max = FName("Rounds_Max");
}

namespace Reticle
{
	const FName RoundedCornerScale = FName("RoundedCornerScale");
	const FName ShapeCutThickness = FName("ShapeCutThickness");
	const FName Inner_RGBA = FName("Inner_RGBA");
}


void UShooterReticle::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	// Hide the image elements to prevent the player from seeing the default ones in the first few frames
	Image_Reticle->SetRenderOpacity(0.0f);
	Image_AmmoCounter->SetRenderOpacity(0.0f);
	
	_BaseCornerScaleFactor_RoundFired = 0.0f;
	_BaseShapeCutFactor_RoundFired = 0.0f;
	_BaseCornerScaleFactor_Aiming = 0.0f;
	_BaseShapeCutFactor_Aiming = 0.0f;
	bAiming = false;
	bTargetingPlayer = false;
	
	// Bind/subscribe our custom OnPossessedPawnChanged function to the event of the same name.
	// The event broadcasts to subscribers when the possessed pawn changes. 
	GetOwningPlayer()->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
	
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) { return; }
	
	OnPossessedPawnChanged(nullptr, ShooterCharacter);
	
	if (ShooterCharacter->HasWeaponFirstReplicated())
	{
		AWeapon* Weapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (IsValid(Weapon))
		{
			OnReticleChanged(Weapon->GetReticleDynamicMaterialInstance(), Weapon->ReticleParams, false);
			OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamicMaterialInstance(), Weapon->Ammo, Weapon->MagCapacity);
		}
	}
	else
	{
		ShooterCharacter->OnWeaponFirstReplicated.AddDynamic(this, &ThisClass::OnWeaponFirstReplicated);
	}
	
	// For hosts (i.e. player on listen server)
	if (ShooterCharacter->HasAuthority())
	{
		AWeapon* Weapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (IsValid(Weapon))
		{
			OnReticleChanged(Weapon->GetReticleDynamicMaterialInstance(), Weapon->ReticleParams, false);
			OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamicMaterialInstance(), Weapon->Ammo, Weapon->MagCapacity);
		}
	}
}

void UShooterReticle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	_BaseCornerScaleFactor_RoundFired = FMath::FInterpTo(_BaseCornerScaleFactor_RoundFired, 0.0f, InDeltaTime, CurrentReticleParams.RoundFiredInterpSpeed);
	_BaseShapeCutFactor_RoundFired = FMath::FInterpTo(_BaseShapeCutFactor_RoundFired, 0.0f, InDeltaTime, CurrentReticleParams.RoundFiredInterpSpeed);
	
	_BaseCornerScaleFactor_Aiming = FMath::FInterpTo(
		_BaseCornerScaleFactor_Aiming,
		bAiming ? CurrentReticleParams.ScaleFactor_Aiming : CurrentReticleParams.ScaleFactor_NotAiming,
		InDeltaTime,
		CurrentReticleParams.AimingInterpSpeed
	);
	
	_BaseShapeCutFactor_Aiming = FMath::FInterpTo(
		_BaseShapeCutFactor_Aiming,
		bAiming ? CurrentReticleParams.ShapeCutFactor_Aiming : CurrentReticleParams.ShapeCutFactor_NotAiming,
		InDeltaTime,
		CurrentReticleParams.AimingInterpSpeed
	);
	
	_BaseCornerScaleFactor_TargetingPlayer = FMath::FInterpTo(
		_BaseCornerScaleFactor_TargetingPlayer,
		bTargetingPlayer ? CurrentReticleParams.ScaleFactor_Targeting : CurrentReticleParams.ScaleFactor_NotTargeting,
		InDeltaTime,
		CurrentReticleParams.TargetingPlayerInterpSpeed
	);
	
	BaseCornerScaleFactor = _BaseCornerScaleFactor_RoundFired + _BaseCornerScaleFactor_Aiming + _BaseCornerScaleFactor_TargetingPlayer;
	BaseShapeCutFactor = _BaseShapeCutFactor_RoundFired + _BaseShapeCutFactor_Aiming;
	
	if (CurrentReticle_DynMatInst.IsValid())
	{
		CurrentReticle_DynMatInst->SetScalarParameterValue(Reticle::RoundedCornerScale, BaseCornerScaleFactor);
		CurrentReticle_DynMatInst->SetScalarParameterValue(Reticle::ShapeCutThickness, BaseShapeCutFactor);
	}
}

void UShooterReticle::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// Unbind from delegates on the OldPawn's Combat Component
	UCombatComponent* OldPawnCombat = UCombatComponent::FindCombatComponent(OldPawn);
	if (IsValid(OldPawnCombat))
	{
		OldPawnCombat->OnReticleChanged.RemoveDynamic(this, &ThisClass::OnReticleChanged);
		OldPawnCombat->OnAmmoCounterChanged.RemoveDynamic(this, &ThisClass::OnAmmoCounterChanged);
		OldPawnCombat->OnRoundFired.RemoveDynamic(this, &ThisClass::OnRoundFired);
		OldPawnCombat->OnAimingStatusChanged.RemoveDynamic(this, &ThisClass::OnAimingStatusChanged);
		OldPawnCombat->OnTargetingPlayerStatusChanged.RemoveDynamic(this, &ThisClass::OnTargetingPlayerStatusChanged);
	}
	
	// Bind to delegates on the NewPawn's Combat Component
	UCombatComponent* NewPawnCombat = UCombatComponent::FindCombatComponent(NewPawn);
	if (IsValid(NewPawnCombat))
	{
		Image_Reticle->SetRenderOpacity(1.0f);
		Image_AmmoCounter->SetRenderOpacity(1.0f);
		
		NewPawnCombat->OnReticleChanged.AddDynamic(this, &ThisClass::OnReticleChanged);
		NewPawnCombat->OnAmmoCounterChanged.AddDynamic(this, &ThisClass::OnAmmoCounterChanged);
		NewPawnCombat->OnRoundFired.AddDynamic(this, &ThisClass::OnRoundFired);
		NewPawnCombat->OnAimingStatusChanged.AddDynamic(this, &ThisClass::OnAimingStatusChanged);
		NewPawnCombat->OnTargetingPlayerStatusChanged.AddDynamic(this, &ThisClass::OnTargetingPlayerStatusChanged);
	}
}

void UShooterReticle::OnWeaponFirstReplicated(AWeapon* Weapon)
{
	OnReticleChanged(Weapon->GetReticleDynamicMaterialInstance(), Weapon->ReticleParams, false);
	OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamicMaterialInstance(), Weapon->Ammo, Weapon->MagCapacity);
}

void UShooterReticle::OnReticleChanged(UMaterialInstanceDynamic* ReticleDynMatInst, const FReticleParams& ReticleParams, bool bCurrentlyTargetingPlayer)
{
	CurrentReticleParams = ReticleParams;
	
	// Set the material of the reticle widget to the dynamic material instance received from the Combat Component's delegate
	CurrentReticle_DynMatInst = ReticleDynMatInst;
	
	FSlateBrush Brush;
	Brush.SetResourceObject(ReticleDynMatInst);
	if (IsValid(Image_Reticle))
	{
		Image_Reticle->SetBrush(Brush);
	}
	
	OnTargetingPlayerStatusChanged(bCurrentlyTargetingPlayer);
}

void UShooterReticle::OnAmmoCounterChanged(UMaterialInstanceDynamic* AmmoCounterDynMatInst, int32 RoundsCurrent, int32 RoundsMax)
{
	// Set the material of the ammo counter widget to the dynamic material instance received from the Combat Component's delegate
	// as well as the material instance's parameters using RoundsCurrent and RoundsMax
	CurrentAmmoCounter_DynMatInst = AmmoCounterDynMatInst;
	CurrentAmmoCounter_DynMatInst->SetScalarParameterValue(Ammo::Rounds_Current, RoundsCurrent);
	CurrentAmmoCounter_DynMatInst->SetScalarParameterValue(Ammo::Rounds_Max, RoundsMax);
	
	FSlateBrush Brush;
	Brush.SetResourceObject(AmmoCounterDynMatInst);
	if (IsValid(Image_AmmoCounter))
	{
		Image_AmmoCounter->SetBrush(Brush);
	}
}

void UShooterReticle::OnRoundFired(int32 RoundsCurrent, int32 RoundsMax)
{
	_BaseCornerScaleFactor_RoundFired += CurrentReticleParams.ScaleFactor_RoundFired;
	_BaseShapeCutFactor_RoundFired += CurrentReticleParams.ShapeCutFactor_RoundFired;
	
	if (CurrentAmmoCounter_DynMatInst.IsValid())
	{
		CurrentAmmoCounter_DynMatInst->SetScalarParameterValue(Ammo::Rounds_Current, RoundsCurrent);
		CurrentAmmoCounter_DynMatInst->SetScalarParameterValue(Ammo::Rounds_Max, RoundsMax);
	}
}

void UShooterReticle::OnAimingStatusChanged(bool bIsAiming)
{
	bAiming = bIsAiming;
}

void UShooterReticle::OnTargetingPlayerStatusChanged(bool bTargeting)
{
	bTargetingPlayer = bTargeting;
	
	if (CurrentReticle_DynMatInst.IsValid())
	{
		FLinearColor ReticleColor = bTargetingPlayer ? FLinearColor::Red : FLinearColor::White;
		CurrentReticle_DynMatInst->SetVectorParameterValue(Reticle::Inner_RGBA, ReticleColor);
	}
}
