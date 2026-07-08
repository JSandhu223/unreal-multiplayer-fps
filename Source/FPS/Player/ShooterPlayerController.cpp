#include "ShooterPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


AShooterPlayerController::AShooterPlayerController()
{
	bReplicates = true;
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(Subsystem))
	{
		Subsystem->AddMappingContext(ShooterIMC, 0);
	}
}

void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(InputComponent);
	
	EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
	EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &ThisClass::Input_Crouch);
	EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Input_Jump);
}

void AShooterPlayerController::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	GetCharacter()->AddMovementInput(ForwardDirection, InputAxisVector.Y); // forward and backward movement
	GetCharacter()->AddMovementInput(RightDirection, InputAxisVector.X); // left and right movement
}

void AShooterPlayerController::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	
	AddPitchInput(InputAxisVector.Y); // looking up and down
	AddYawInput(InputAxisVector.X); // looking left and right
}

void AShooterPlayerController::Input_Crouch()
{
	if (!IsValid(GetCharacter()))
	{
		return;
	}
	
	if (UCharacterMovementComponent* MovementComp = GetCharacter()->GetCharacterMovement())
	{
		MovementComp->bWantsToCrouch = !MovementComp->bWantsToCrouch;
	}
}

void AShooterPlayerController::Input_Jump()
{
	if (!IsValid(GetCharacter()))
	{
		return;
	}
	
	UCharacterMovementComponent* MovementComp = GetCharacter()->GetCharacterMovement();
	if (!IsValid(MovementComp))
	{
		return;
	}
	
	if (MovementComp->bWantsToCrouch)
	{
		// Uncrouch if player presses jump input while crouching
		MovementComp->bWantsToCrouch = false;
	}
	else
	{
		GetCharacter()->Jump();
	}
}
