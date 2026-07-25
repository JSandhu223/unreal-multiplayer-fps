#include "ShooterReticle.h"

#include "Character/ShooterCharacter.h"
#include "Combat/CombatComponent.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Weapon/Weapon.h"


void UShooterReticle::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
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
			OnReticleChanged(Weapon->GetReticleDynamicMaterialInstance());
			OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamicMaterialInstance(), Weapon->Ammo, Weapon->MagCapacity);
		}
	}
	else
	{
		ShooterCharacter->OnWeaponFirstReplicated.AddDynamic(this, &ThisClass::UShooterReticle::OnWeaponFirstReplicated);
	}
	
	// For hosts (i.e. player on listen server)
	if (ShooterCharacter->HasAuthority())
	{
		AWeapon* Weapon = IPlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (IsValid(Weapon))
		{
			OnReticleChanged(Weapon->GetReticleDynamicMaterialInstance());
			OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamicMaterialInstance(), Weapon->Ammo, Weapon->MagCapacity);
		}
	}
}

void UShooterReticle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UShooterReticle::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// Unbind from delegates on the OldPawn's Combat Component
	UCombatComponent* OldPawnCombat = UCombatComponent::FindCombatComponent(OldPawn);
	if (IsValid(OldPawnCombat))
	{
		OldPawnCombat->OnReticleChanged.RemoveDynamic(this, &ThisClass::OnReticleChanged);
		OldPawnCombat->OnAmmoCounterChanged.RemoveDynamic(this, &ThisClass::OnAmmoCounterChanged);
	}
	
	// Bind to delegates on the NewPawn's Combat Component
	UCombatComponent* NewPawnCombat = UCombatComponent::FindCombatComponent(NewPawn);
	if (IsValid(NewPawnCombat))
	{
		NewPawnCombat->OnReticleChanged.AddDynamic(this, &ThisClass::OnReticleChanged);
		NewPawnCombat->OnAmmoCounterChanged.AddDynamic(this, &ThisClass::OnAmmoCounterChanged);
	}
}

void UShooterReticle::OnWeaponFirstReplicated(AWeapon* Weapon)
{
	OnReticleChanged(Weapon->GetReticleDynamicMaterialInstance());
	OnAmmoCounterChanged(Weapon->GetAmmoCounterDynamicMaterialInstance(), Weapon->Ammo, Weapon->MagCapacity);
}

void UShooterReticle::OnReticleChanged(UMaterialInstanceDynamic* ReticleDynMatInst)
{
	// Set the material of the reticle widget to the dynamic material instance received from the Combat Component's delegate
	CurrentReticle_DynMatInst = ReticleDynMatInst;
	
	FSlateBrush Brush;
	Brush.SetResourceObject(ReticleDynMatInst);
	
	if (IsValid(Image_Reticle))
	{
		Image_Reticle->SetBrush(Brush);
	}
}

void UShooterReticle::OnAmmoCounterChanged(UMaterialInstanceDynamic* AmmoCounterDynMatInst, int32 RoundsCurrent, int32 RoundsMax)
{
	// Set the material of the ammo counter widget to the dynamic material instance received from the Combat Component's delegate
	CurrentAmmoCounter_DynMatInst = AmmoCounterDynMatInst;
	
	FSlateBrush Brush;
	Brush.SetResourceObject(AmmoCounterDynMatInst);
	
	if (IsValid(Image_AmmoCounter))
	{
		Image_AmmoCounter->SetBrush(Brush);
	}
}
