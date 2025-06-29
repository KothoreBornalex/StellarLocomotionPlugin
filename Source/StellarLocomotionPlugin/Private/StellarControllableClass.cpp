// Fill out your copyright notice in the Description page of Project Settings.


#include "StellarControllableClass.h"


#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


// Sets default values
AStellarControllableClass::AStellarControllableClass()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	EntityPlayerController = nullptr;
}

// Called when the game starts or when spawned
void AStellarControllableClass::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStellarControllableClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStellarControllableClass::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Parameters;
	Parameters.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, EntityPlayerController, Parameters);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Environement, Parameters);
}


#pragma region Ownership Functions
void AStellarControllableClass::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	SetController(NewOwner);
}

void AStellarControllableClass::OnRep_Owner()
{
	Super::OnRep_Owner();
}

void AStellarControllableClass::SetController(AActor* PlayerController)
{
	if(!HasAuthority()) return;
	if(!PlayerController) return;
	
	EntityPlayerController = Cast<APlayerController>(PlayerController);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, EntityPlayerController, this)
}

APlayerController* AStellarControllableClass::GetController()
{
	if(EntityPlayerController)
	{
		return EntityPlayerController;
	}
	
	return nullptr;
}



void AStellarControllableClass::SetEnvironment(AActor* NewEnvironment)
{
	Environement = NewEnvironment;
	OnReplicated_Environement();
}

void AStellarControllableClass::OnReplicated_Environement()
{
	
}


void AStellarControllableClass::OnReplicated_EntityPlayerController()
{
	if(!GetController()->IsLocalController() || HasAuthority()) return;
	
	auto* NewPlayer{ GetController() };
	if (IsValid(NewPlayer))
	{
		NewPlayer->InputYawScale_DEPRECATED = 1.0f;
		NewPlayer->InputPitchScale_DEPRECATED = 1.0f;
		NewPlayer->InputRollScale_DEPRECATED = 1.0f;

		auto* InputSubsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayer->GetLocalPlayer()) };
		if (IsValid(InputSubsystem))
		{
			FModifyContextOptions Options;
			Options.bNotifyUserSettings = true;

			InputSubsystem->AddMappingContext(InputMappingContext, 0, Options);
		}
	}

	SetupPlayerInputComponent(EntityPlayerController->InputComponent);
}
#pragma endregion




#pragma region Inputs Functions

void AStellarControllableClass::AddControllerPitchInput(float Val)
{
	if (Val != 0.f && GetController() && GetController()->IsLocalPlayerController())
	{
		AddPitchInput(Val);
	}
}

void AStellarControllableClass::AddControllerYawInput(float Val)
{
	if (Val != 0.f && GetController() && GetController()->IsLocalPlayerController())
	{
		AddYawInput(Val);
	}
}

void AStellarControllableClass::AddControllerRollInput(float Val)
{
	if (Val != 0.f && GetController() && GetController()->IsLocalPlayerController())
	{
		AddRollInput(Val);
	}
}


void AStellarControllableClass::AddPitchInput(float Val)
{	
	RotationInput.Pitch += !GetController()->IsLookInputIgnored() ? Val * 1.0f : 0.0f;
}

void AStellarControllableClass::AddYawInput(float Val)
{
	RotationInput.Yaw += !GetController()->IsLookInputIgnored() ? Val *  1.0f : 0.0f;
}

void AStellarControllableClass::AddRollInput(float Val)
{
	RotationInput.Roll += !GetController()->IsLookInputIgnored() ? Val * 1.0f : 0.0f;
}


// Called to bind functionality to input
void AStellarControllableClass::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("SET PLAYER INPUT COMPONENT"));
	}
	
}


#pragma endregion
