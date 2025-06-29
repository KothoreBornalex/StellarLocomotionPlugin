// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StellarControllableClass.generated.h"

class UInputMappingContext;

UCLASS()
class STELLARLOCOMOTIONPLUGIN_API AStellarControllableClass : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Transient, ReplicatedUsing = "OnReplicated_Environement")
	TObjectPtr<AActor> Environement;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, ReplicatedUsing = "OnReplicated_EntityPlayerController")
	APlayerController* EntityPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inputs", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputMappingContext> InputMappingContext;

protected:
	FVector MovementInput;
	FRotator RotationInput;

	
public:
	// Sets default values for this actor's properties
	AStellarControllableClass();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

#pragma region Ownership Functions
	virtual void SetOwner(AActor* NewOwner) override;
	virtual void OnRep_Owner() override;
	virtual void SetController(AActor* PlayerController);
	virtual APlayerController* GetController();

	UFUNCTION(BlueprintCallable)
	void SetEnvironment(AActor* NewEnvironment);
	UFUNCTION()
	virtual void OnReplicated_Environement();
	UFUNCTION()
	void OnReplicated_EntityPlayerController();
#pragma endregion

	


#pragma region Inputs Functions
	

protected:
	void AddControllerPitchInput(float Val);
	void AddControllerYawInput(float Val);
	void AddControllerRollInput(float Val);
	
	void AddPitchInput(float Val);
	void AddYawInput(float Val);
	void AddRollInput(float Val);
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);

#pragma endregion
};
