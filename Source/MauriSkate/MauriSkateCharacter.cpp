// Copyright Epic Games, Inc. All Rights Reserved.

#include "MauriSkateCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MauriSkate.h"
#include "Chaos/Utilities.h"
#include "Kismet/GameplayStatics.h"

void AMauriSkateCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SkatePushRemainingTime = FMath::Clamp(SkatePushRemainingTime-DeltaSeconds,0.0,FLT_MAX);

	IsJumping = IsJumpingNow();
	IsPushing = IsPushingNow();
	
	if (IsPushing &&
		!PushingInstantReached &&
		(SkatePushRemainingTime <= (1-SkatePushAnimationInstantNormalized)*SkatePushAnimationDuration)
		)
	{
		DoPush(1.0);
		PushingInstantReached = true;
	} else if (IsJumping)
	{
		// cancels pushng if starts to be in the air
		SkatePushRemainingTime = 0.0;
	}

	if (SkatePushRemainingTime <= 0)
	{
		PushingInstantReached = false;
		IsPushing = false;
	}

	if (GetCharacterMovement()->IsMovingOnGround())
	{
		const FFindFloorResult Floor = GetCharacterMovement()->CurrentFloor;
		const FVector FloorNormal = Floor.HitResult.ImpactNormal;
		const FVector FloorPoint = Floor.HitResult.ImpactPoint;

		if (FloorNormal.Dot(FVector::UpVector) < 0.98)
		{
			
			const FVector Ramp = (FloorNormal * FVector(1.0,1.0,0.00)).GetSafeNormal(0.001);
			const FVector DownsideFacing =
				(Ramp.Dot(GetActorForwardVector()) > Ramp.Dot(GetActorForwardVector()*(-1.0))) ?
			GetActorForwardVector() : GetActorForwardVector()*(-1.0);

			const FVector Inclination = FVector::VectorPlaneProject(DownsideFacing, FloorNormal).GetSafeNormal(0.001);
			const FVector FinalForce =
				Inclination * (Inclination.Dot(FVector::DownVector) * GetWorld()->GetGravityZ() * -1.0 * GetCharacterMovement()->Mass * SkateGravityFactor);

			GetCharacterMovement()->AddForce(FinalForce);

			/*GEngine->AddOnScreenDebugMessage(-1, 0.25f, FColor::Blue,
				FString::Printf(TEXT("Ramp:%s ; Downside:%s ; Inclination:%s ; Final:%s"),
					*(Ramp.ToString()),
					*(DownsideFacing.ToString()),
					*(Inclination.ToString()),
					*(FinalForce.ToString())
					));*/
		}
	}
	
}

AMauriSkateCharacter::AMauriSkateCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 150.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 0.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 0.0f;
	GetCharacterMovement()->GroundFriction = SkateFloorFriction;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AMauriSkateCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMauriSkateCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMauriSkateCharacter::StopJumping);

		// Pushing
		EnhancedInputComponent->BindAction(PushAction, ETriggerEvent::Completed, this, &AMauriSkateCharacter::Push);
		
		EnhancedInputComponent->BindAction(SlowDownAction, ETriggerEvent::Started, this, &AMauriSkateCharacter::SlowDownStarted);
		EnhancedInputComponent->BindAction(SlowDownAction, ETriggerEvent::Completed, this, &AMauriSkateCharacter::SlowDownStopped);
		
		// Turning
		EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AMauriSkateCharacter::Turn);

		// look
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMauriSkateCharacter::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMauriSkateCharacter::Look);
	}
	else
	{
		UE_LOG(LogMauriSkate, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMauriSkateCharacter::Turn(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	// route the input
	DoTurn(MovementVector.X, MovementVector.Y);
}

void AMauriSkateCharacter::Push(const FInputActionValue& Value)
{
	bool ActionValue = Value.Get<bool>();
	
	if (SkatePushRemainingTime <= 0.0 && !IsJumpingNow())
	{
		SkatePushRemainingTime = SkatePushAnimationDuration;
	}
}

void AMauriSkateCharacter::SlowDownStarted(const FInputActionValue& Value)
{
	const bool ActionValue = Value.Get<bool>();
	
	if (!IsPushingNow() && !IsJumpingNow())
	{
		DoSlowDown(true);
	}
}

void AMauriSkateCharacter::SlowDownStopped(const FInputActionValue& Value)
{
	const bool ActionValue = Value.Get<bool>();
	
	DoSlowDown(false);
}

void AMauriSkateCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMauriSkateCharacter::DoTurn(float Right, float Forward)
{
	if (GetController() != nullptr && !IsJumpingNow())
	{

		const FRotator InitialRotation = GetCharacterMovement()->UpdatedComponent->GetComponentRotation();
		const FRotator InitialYawRotation(0,InitialRotation.Yaw,0);
		const FVector InitialForwardDirection = FRotationMatrix(InitialYawRotation).GetUnitAxis(EAxis::X);
		
		const FRotator ControllerRotation = GetController()->GetControlRotation();
		const FRotator ControllerYawRotation(0, ControllerRotation.Yaw, 0);
		const FVector TargetForwardDirection = FRotationMatrix(ControllerYawRotation).GetUnitAxis(EAxis::X);
		const FVector TargetRightDirection = FRotationMatrix(ControllerYawRotation).GetUnitAxis(EAxis::Y);
		const FVector TargetFinalDirection = (TargetForwardDirection*Forward + TargetRightDirection*Right).GetSafeNormal(0.001);
		const FRotator TargetRotator = TargetFinalDirection.Rotation();

		// RInterpConstantTo uses Degrees
		// RInterpTo uses a relative measure
		const FRotator NewRotation = FMath::RInterpConstantTo(InitialYawRotation,TargetRotator,UGameplayStatics::GetWorldDeltaSeconds(this),SkateRelativeTurningSpeed*360);

		// Rotating the pawn
		GetCharacterMovement()->MoveUpdatedComponent(FVector::ZeroVector,NewRotation,false);
		
		// Updating the velocity Direction
		const FVector OldHorizontalVelocity = GetCharacterMovement()->Velocity * FVector(1.0,1.0,0.0);
		const FVector OldVerticalVelocity = GetCharacterMovement()->Velocity * FVector(0.0,0.0,1.0);
		const float OldHorizontalSpeed = OldHorizontalVelocity.Length();
		const FVector NewVelocity = FRotationMatrix(NewRotation).GetUnitAxis(EAxis::X)*OldHorizontalSpeed + OldVerticalVelocity;
		GetCharacterMovement()->Velocity = NewVelocity;
		
	}
}

void AMauriSkateCharacter::DoPush(float Factor)
{
	GEngine->AddOnScreenDebugMessage(-1,15.0f,FColor::Magenta, TEXT("Added Push"));
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetActorRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement

		GetCharacterMovement()->AddImpulse(ForwardDirection*SkatePushForce*Factor);
		//AddMovementInput(ForwardDirection, Forward);
		//AddMovementInput(RightDirection, Right);
		
	}
}

void AMauriSkateCharacter::DoSlowDown(const bool Pressed)
{
	if (Pressed)
	{
		GEngine->AddOnScreenDebugMessage(-1,15.0f,FColor::Magenta, TEXT("Slow down started"));
		GetCharacterMovement()->GroundFriction = SkateSlowDownFriction;
	} else
	{
		GEngine->AddOnScreenDebugMessage(-1,15.0f,FColor::Magenta, TEXT("SlowDown ended"));
		GetCharacterMovement()->GroundFriction = SkateFloorFriction;
	}
}

void AMauriSkateCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMauriSkateCharacter::DoJumpStart()
{
	if (!IsPushingNow())
	{
		Jump();
	}
}

void AMauriSkateCharacter::DoJumpEnd()
{
	StopJumping();
}

bool AMauriSkateCharacter::IsPushingNow() const
{
	return (SkatePushRemainingTime > 0.0);
}

bool AMauriSkateCharacter::IsJumpingNow() const
{
	return not this->GetCharacterMovement()->IsMovingOnGround();
}
