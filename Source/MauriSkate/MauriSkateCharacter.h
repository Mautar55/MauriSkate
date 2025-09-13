// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MauriSkateCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AMauriSkateCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	virtual void Tick(float DeltaSeconds) override;
	
protected:
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* PushAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

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

protected:

	/** Called for turning the skate input */
	void Turn(const FInputActionValue& Value);

	/** Called for impulse input */
	void Push(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:
	/** Handles turning/rotation inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoTurn(float Right, float Forward);

	/** Handles turning/rotation inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoPush(float Factor);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);
	
	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public: // Editable skate animation parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	float SkatePushAnimationDuration = 2.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	float SkatePushAnimationInstantNormalized = 0.5769;

	// Read only are exposed for the animation BP
	UPROPERTY(BlueprintReadOnly, Category = "Skate")
	bool IsPushing = false;
	UPROPERTY(BlueprintReadOnly, Category = "Skate")
	bool IsJumping = false;

	// Editable Skate character parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	float SkatePushForce = 50000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	float MaxSkateHorizontalVelocity = 1000.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skate")
	float SkateRelativeTurningSpeed = 1.0;
	
	bool IsPushingNow() const;
	bool IsJumpingNow() const;

private: // Inner Working skate state variables
	float SkatePushRemainingTime = 0.0;
	bool PushingInstantReached = false;

	float PendingSolvingSkateForce = 0.0;
	
	void AddSkateImpulse(float ImpulseIntensity)
	{
		PendingSolvingSkateForce += ImpulseIntensity;
	}

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

