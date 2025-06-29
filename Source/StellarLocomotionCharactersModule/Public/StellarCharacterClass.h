// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StellarCharacterMotor.h"
#include "Interfaces/StellarCharacterController.h"
#include "StellarLocomotionPlugin/Public/StellarControllableClass.h"

#include "StellarCharacterClass.generated.h"

class UStellarCharacterMotor;
class UCameraComponent;
class UCapsuleComponent;

UCLASS()
class STELLARLOCOMOTIONCHARACTERSMODULE_API AStellarCharacterClass : public AStellarControllableClass, public IStellarCharacterController
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule;

	UPROPERTY()
	TObjectPtr<UStellarCharacterMotor> CharacterMotor;

	UPROPERTY()
	FStellarPostPhysicTick PostPhysicTick_Delegate;

	


#pragma region Movements Global Fields
	
public:
	float CurrentMaxSpeed = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character Fields|Speed", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float WalkForwardSpeed{175.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character Fields|Speed", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float JogForwardSpeed{375.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character Fields|Speed", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float SprintSpeed{650.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character Fields|Speed", Meta = (ClampMin = 0, ForceUnits = "cm/s"))
	float CrouchSpeed{150.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character Fields|Height", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float StandHeight{88.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character Fields|Height", Meta = (ClampMin = 0, ForceUnits = "cm"))
	float CrouchHeight{55.0f};

#pragma endregion
	
public:
	// Sets default values for this actor's properties
	AStellarCharacterClass();

protected:
	// Called when the game starts or when spawned
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnReplicated_Environement() override;
	
	void SetMaxSpeed(const float& NewMaxSpeed, bool bSendRpc = true);

#pragma region IStellarCharacterController
public:
	void UpdateVelocity_Implementation(FVector& gravityVelocity, FVector& movementVelocity, float& deltaTime);
	void UpdateRotation_Implementation(FQuat& currentRotation, float& deltaTime);

#pragma endregion
};
