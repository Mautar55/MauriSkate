// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"
#include "Logging/LogMacros.h"
#include "MauriSkateCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract, meta = (PrioritizeCategories ="Skate"))
class AMauriSkateCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SkateMesh;

	virtual void Tick(float DeltaSeconds) override;
	
protected:
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* PushAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SlowDownAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* TurnAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

public:

	/** Constructor */
	AMauriSkateCharacter();	

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected: // Bound to input actions

	void Turn(const FInputActionValue& Value);

	void Push(const FInputActionValue& Value);

	void SlowDownStarted(const FInputActionValue& Value);
	void SlowDownStopped(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

public: // Inputs from either controls or UI interfaces
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoTurn(float Right, float Forward);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoPush(float Factor);

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoSlowDown(bool Pressed);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);
	
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public: // Editable skate animation parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	float SkatePushAnimationDuration = 2.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	float SkatePushAnimationInstantNormalized = 0.5769;

	// Read only are exposed for the animation BP
	UPROPERTY(BlueprintReadOnly, Category = "Skate")
	bool bIsPushing = false;
	UPROPERTY(BlueprintReadOnly, Category = "Skate")
	bool bIsJumping = false;

	// Editable Skate character parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	float SkatePushForce = 50000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	float MaxSkateHorizontalVelocity = 1000.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	float SkateRelativeTurningSpeed = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	float SkateGravityFactor = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	float SkateFloorFriction = 0.08;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	float SkateSlowDownFriction = 4.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	UMaterialInterface *SkateReadyMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	UMaterialInterface *SkateExecutingMaterial;
	
	bool IsPushingNow() const;
	bool IsJumpingNow() const;

	void KillCharacter();

private: // Inner Working skate state variables
	float SkatePushRemainingTime = 0.0;
	bool bPushingInstantReached = false;

	float PendingSolvingSkateForce = 0.0;

	bool bIsSlowingDown = false;
	
	void AddSkateImpulse(float ImpulseIntensity);

	void SetSkateAlternativeMaterial(const bool SetAlternative);


public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

