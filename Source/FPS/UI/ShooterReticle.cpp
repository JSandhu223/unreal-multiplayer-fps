#include "ShooterReticle.h"

#include "Character/ShooterCharacter.h"


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
		// Get Dynamic Material Instances from the Weapon
	}
	else
	{
		ShooterCharacter->OnWeaponFirstReplicated.AddDynamic(this, &ThisClass::UShooterReticle::OnWeaponFirstReplicated);
	}
}

void UShooterReticle::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UShooterReticle::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// Unbind from delegates on the OldPawn's Combat Component
	// Bind to delegates on the NewPawn's Combat Component
}

void UShooterReticle::OnWeaponFirstReplicated(AWeapon* Weapon)
{
	// Get Dynamic Material Instances from the Weapon
}
