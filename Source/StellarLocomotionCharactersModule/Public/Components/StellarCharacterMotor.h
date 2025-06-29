// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StellarCharacterMotor.generated.h"

class UCapsuleComponent;

DECLARE_DYNAMIC_DELEGATE_OneParam(FStellarPostPhysicTick, float, DeltaTime);

/**
 * 
 */
UCLASS()
class STELLARLOCOMOTIONCHARACTERSMODULE_API UStellarCharacterMotor : public UObject
{
	GENERATED_BODY()

private:
	FTSTicker::FDelegateHandle TickHandle;
	
	FDelegateHandle PostActorTickHandle;

	UPROPERTY()
	TObjectPtr<AActor> Environement;

	UPROPERTY()
	FTransform InitialRelativeTransform;

	UPROPERTY()
	FTransform CurrentOffsetTransform;
	
	UPROPERTY()
	FTransform LocalOffsetTransform;

	UPROPERTY()
	FTransform SimulatedChildTransform;

	UPROPERTY()
	FTransform FinalTransform;
	
	UPROPERTY()
	AActor* _owner;
	
	UPROPERTY()
	UCapsuleComponent* _bounds;

	// UPROPERTY()
	// FBoxSphereBounds _boundingBox;
	
	UPROPERTY()
	FVector _movementVelocity;
	
	UPROPERTY()
	FVector _gravityVelocity;
	
	UPROPERTY()
	FVector _finalVelocity;

	UPROPERTY()
	FQuat _rotation;

	UPROPERTY()
	bool _isGrounded;
	
	UPROPERTY()
	int _maxBounces = 5;

	UPROPERTY()
	float _maxSlopeAngle = 45.0f;
	
	UPROPERTY()
	float _skinWidth = 1.5f;

	UPROPERTY()
	float StepHeight = 40.0f;

	UPROPERTY()
	float StepCheckDistance = 5.0f;




	
public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsGrounded() const { return _isGrounded; }


public:
	void StartTicking();
	void StopTicking();

private:
	UFUNCTION()
	void Tick(float DeltaTime);

	virtual class UWorld* GetWorld() const override;

public:
	void InitCharacterMotor(AActor* owner, UCapsuleComponent* bounds, FStellarPostPhysicTick& TickDelegate);

	
public:
	UFUNCTION()
	void SetCharacterEnvironement(AActor* env);
	UFUNCTION()
	void UpdateTransform(float DeltaTime);
	UFUNCTION()
	void UpdateRotation(float DeltaTime);
	UFUNCTION()
	void UpdateMovements(float DeltaTime);
	UFUNCTION()
	void ApplyTransform(float DeltaTime);

	UFUNCTION()
	FVector GetGlobalForwardVector();
	UFUNCTION()
	FVector GetGlobalRightVector();
	UFUNCTION()
	FVector GetGlobalUpVector();

	UFUNCTION()
	FVector GetCharacterForwardVector();
	UFUNCTION()
	FVector GetCharacterRightVector();
	UFUNCTION()
	FVector GetCharacterUpVector();

	
	UFUNCTION()
	void Dev_UpdateMovement(FVector MovementInput);

	
public:
	bool IsCharacterGrounded();

	FVector CollideAndSlide(FVector vel, FVector initialVel, FVector pos, int depth, bool gravityPass);
	FVector HomeMadeCollideAndSlide(FVector vel, FVector initialVel, FVector pos, int depth, bool gravityPass);

	
	FHitResult CheckCapsuleCollision(FVector Start, FVector End, float CapsuleRadius, float CapsuleHalfHeight, FQuat CapsuleRotation, bool drawDebug = false);

	FVector ProjectAndScale(FVector vec, FVector normal);
	bool IsStepSurface(const FVector& Normal, const FVector& Velocity);
	bool TryStepUp(UWorld* World, const FVector& CurrentPos, const FVector& InputVel, FVector& OutFinalPos);
};
