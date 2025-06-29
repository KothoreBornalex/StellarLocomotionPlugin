// Fill out your copyright notice in the Description page of Project Settings.


#include "StellarCharacterClass.h"

#include "Components/CapsuleComponent.h"
#include "Components/StellarCharacterMotor.h"
// Sets default values
AStellarCharacterClass::AStellarCharacterClass()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostPhysics;
	bReplicates = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCapsuleHalfHeight(88.0f);
	Capsule->SetCapsuleRadius(34.0f);
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Capsule->SetCollisionObjectType(ECC_Pawn);
	Capsule->SetCollisionResponseToAllChannels(ECR_Block);
	Capsule->PrimaryComponentTick.TickGroup = TG_PostPhysics;


	RootComponent = Capsule;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SkeletalMeshComponent->SetRelativeLocation(FVector(0,0,-90.0f));
	SkeletalMeshComponent->SetRelativeRotation(FRotator(0,-90.0f,0));
	SkeletalMeshComponent->PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

void AStellarCharacterClass::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CharacterMotor = NewObject<UStellarCharacterMotor>(this);
	CharacterMotor->InitCharacterMotor(this, Capsule, PostPhysicTick_Delegate);
}

// Called when the game starts or when spawned
void AStellarCharacterClass::BeginPlay()
{
	Super::BeginPlay();
	SetMaxSpeed(WalkForwardSpeed);
}

// Called every frame
void AStellarCharacterClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PostPhysicTick_Delegate.Execute(DeltaTime);
}

void AStellarCharacterClass::OnReplicated_Environement()
{
	Super::OnReplicated_Environement();

	if(GetOwner()) CharacterMotor->SetCharacterEnvironement(Environement);
}


void AStellarCharacterClass::SetMaxSpeed(const float& NewMaxSpeed, bool bSendRpc)
{
	if (CurrentMaxSpeed != NewMaxSpeed)
	{
		CurrentMaxSpeed = NewMaxSpeed;
	}
}


void AStellarCharacterClass::UpdateVelocity_Implementation(FVector& gravityVelocity, FVector& movementVelocity,
	float& deltaTime)
{

	const auto ForwardDirection{ CharacterMotor->GetCharacterForwardVector() };
	const auto RightDirection{ CharacterMotor->GetCharacterRightVector() };

	FVector MoveDir = (ForwardDirection * MovementInput.Y + RightDirection * MovementInput.X).GetSafeNormal();
	movementVelocity = MoveDir * CurrentMaxSpeed * deltaTime;
	
	if(CharacterMotor->IsGrounded())
	{
		gravityVelocity = FVector::ZeroVector;
	}else
	{
		gravityVelocity = CharacterMotor->GetGlobalUpVector() * -980.0f * deltaTime;
	}
	
	MovementInput = FVector::ZeroVector;
}

void AStellarCharacterClass::UpdateRotation_Implementation(FQuat& currentRotation, float& deltaTime)
{
	FRotator NewRotation = RotationInput * 45.0f * deltaTime;
	currentRotation = NewRotation.Quaternion();
	RotationInput = FRotator::ZeroRotator;
}

