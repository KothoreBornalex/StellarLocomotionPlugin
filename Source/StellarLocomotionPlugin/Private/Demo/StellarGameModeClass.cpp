// Fill out your copyright notice in the Description page of Project Settings.


#include "Demo/StellarGameModeClass.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"


void AStellarGameModeClass::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(NewPlayer)
	{
		GetBasePlayerPawn(NewPlayer);
	}
}

FVector AStellarGameModeClass::FindRandomPlayerStartLocation()
{
	// The class of actors you want to retrieve
	TSubclassOf<APlayerStart> ActorClass = APlayerStart::StaticClass();

	TArray<AActor*> FoundActors;

	// Retrieve all actors of the specified class
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ActorClass, FoundActors);

	int Index = FMath::RandRange(0, FoundActors.Num() - 1);

	if(IsValid(FoundActors[Index]))
	{
		return FoundActors[Index]->GetActorLocation();
	}else
	{
		return FVector();
	}
}


AActor* AStellarGameModeClass::GetBasePlayerPawn(APlayerController* TargetedController)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.Instigator = TargetedController->GetInstigator();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	TObjectPtr<AActor> PawnActor = Cast<AActor>(GetWorld()->SpawnActor(EntityCharacterClass, nullptr, nullptr, SpawnParameters));
	PawnActor->SetActorLocation(FindRandomPlayerStartLocation());

	PawnActor->SetOwner(TargetedController);
	TargetedController->SetViewTarget(PawnActor);

	return PawnActor;
}