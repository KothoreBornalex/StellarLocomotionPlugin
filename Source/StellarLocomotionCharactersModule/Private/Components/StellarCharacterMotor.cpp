// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/StellarCharacterMotor.h"

#include "Components/CapsuleComponent.h"
#include "Interfaces/StellarCharacterController.h"


void UStellarCharacterMotor::StartTicking()
{
	// if (!TickHandle.IsValid())
	// {
	// 	TickHandle = FTSTicker::GetCoreTicker().AddTicker(
	// 		FTickerDelegate::CreateUObject(this, &UCharacterMotor::Tick),
	// 		0.0f // Tick every frame
	// 	);
	// }
}

void UStellarCharacterMotor::StopTicking()
{
	// if (TickHandle.IsValid())
	// {
	// 	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
	// 	TickHandle.Reset();
	// }
}

void UStellarCharacterMotor::Tick(float DeltaTime)
{
	_isGrounded = IsCharacterGrounded();
	
	// Your tick logic here
	if (_owner->Implements<UStellarCharacterController>())
	{
		IStellarCharacterController::Execute_UpdateRotation(_owner, _rotation, DeltaTime);
		IStellarCharacterController::Execute_UpdateVelocity(_owner, _gravityVelocity, _movementVelocity, DeltaTime);
	}
	
	UpdateTransform(DeltaTime);
	UpdateRotation(DeltaTime);
	UpdateMovements(DeltaTime);
	ApplyTransform(DeltaTime);

	
	//return true; // Keep ticking
}

class UWorld* UStellarCharacterMotor::GetWorld() const
{
	if(_owner) return _owner->GetWorld();
	else
	{
		return nullptr;
	}
}

void UStellarCharacterMotor::InitCharacterMotor(AActor* owner, UCapsuleComponent* bounds, FStellarPostPhysicTick& TickDelegate)
{
	_owner = owner;
	_bounds = bounds;
	TickDelegate.BindDynamic(this, &UStellarCharacterMotor::Tick);

	//StartTicking();
}

void UStellarCharacterMotor::SetCharacterEnvironement(AActor* env)
{
	Environement = env;
	InitialRelativeTransform = Environement ? _owner->GetActorTransform().GetRelativeTransform(Environement->GetActorTransform()) : FTransform::Identity;

	FString Msg = Environement ? "Environement is Valid" : "Environement is Invalid"; 
	UE_LOG(LogTemp, Log, TEXT("%s"), *Msg);
}

void UStellarCharacterMotor::UpdateTransform(float DeltaTime)
{
	if (!Environement) return;
	
	const FTransform& ParentWorldTransform = Environement->GetActorTransform();
	
	SimulatedChildTransform = InitialRelativeTransform * ParentWorldTransform;
	FinalTransform = LocalOffsetTransform * SimulatedChildTransform;
}

void UStellarCharacterMotor::UpdateRotation(float DeltaTime)
{
	FRotator DeltaRot(_rotation);
	FRotator ActorRotation = Environement ? LocalOffsetTransform.GetRotation().Rotator() : _owner->GetActorRotation();
	
	ActorRotation.Yaw += DeltaRot.Yaw;
	
	LocalOffsetTransform.SetRotation(ActorRotation.Quaternion());
	CurrentOffsetTransform.SetRotation(ActorRotation.Quaternion());
	_rotation = FQuat::Identity;
}

void UStellarCharacterMotor::UpdateMovements(float DeltaTime)
{
	//FVector locomotionVelocity = _gravityVelocity;
	//locomotionVelocity = CollideAndSlide(locomotionVelocity, locomotionVelocity, _owner->GetActorLocation(), 0, false);
	
	// if(_isGrounded)
	// {
	// 	_finalVelocity = CollideAndSlide(_movementVelocity, _movementVelocity, _owner->GetActorLocation(), 0, false);
	// }else
	// {
	// 	_movementVelocity = CollideAndSlide(_movementVelocity, _movementVelocity, _owner->GetActorLocation(), 0, false);
	// 	_gravityVelocity = CollideAndSlide(_gravityVelocity, _gravityVelocity, _owner->GetActorLocation(), 0, true);
	// 	_finalVelocity = _gravityVelocity + _movementVelocity;
	// }
	//
	// _owner->SetActorLocation(_owner->GetActorLocation() + _finalVelocity);

	
	// _movementVelocity = CollideAndSlide(_movementVelocity, _movementVelocity, _owner->GetActorLocation(), 0, false);
	// _gravityVelocity = CollideAndSlide(_gravityVelocity, _gravityVelocity, _owner->GetActorLocation(), 0, true);
	//
	// _finalVelocity = _gravityVelocity + _movementVelocity;
	// _owner->SetActorLocation(_owner->GetActorLocation() + _movementVelocity);


	//_movementVelocity = HomeMadeCollideAndSlide(_movementVelocity, _movementVelocity, _owner->GetActorLocation(), 0, false);
	//_movementVelocity += HomeMadeCollideAndSlide(_gravityVelocity, _gravityVelocity, _owner->GetActorLocation() + _movementVelocity, 0, true);
	
	//_owner->SetActorLocation(_owner->GetActorLocation() + _movementVelocity);

	if(Environement)
	{
		FVector EndMovementVector = LocalOffsetTransform.GetLocation();
		
		_movementVelocity = HomeMadeCollideAndSlide(_movementVelocity, _movementVelocity, FinalTransform.GetLocation(), 0, false);
		//_movementVelocity += HomeMadeCollideAndSlide(_gravityVelocity, _gravityVelocity, FinalTransform.GetLocation() + _movementVelocity, 0, true);

		EndMovementVector += _movementVelocity;
		LocalOffsetTransform.SetLocation(EndMovementVector);
	}else
	{
		FVector EndMovementVector = _owner->GetActorLocation();
	
		_movementVelocity = HomeMadeCollideAndSlide(_movementVelocity, _movementVelocity, _owner->GetActorLocation(), 0, false);
		_movementVelocity += HomeMadeCollideAndSlide(_gravityVelocity, _gravityVelocity, _owner->GetActorLocation() + _movementVelocity, 0, true);

		EndMovementVector += _movementVelocity;
		CurrentOffsetTransform.SetLocation(EndMovementVector);
	}
	
}

void UStellarCharacterMotor::ApplyTransform(float DeltaTime)
{
	if (Environement)
	{
		FinalTransform = LocalOffsetTransform * SimulatedChildTransform;
	
		// Apply user movement & yaw offset on top
		_owner->SetActorTransform(FinalTransform);
	}
	else
	{
		_owner->SetActorTransform(CurrentOffsetTransform);
	}
}

FVector UStellarCharacterMotor::GetGlobalForwardVector()
{
	return Environement ? FinalTransform.GetUnitAxis(EAxis::X) : _owner->GetActorForwardVector();
}

FVector UStellarCharacterMotor::GetGlobalRightVector()
{
	return Environement ? FinalTransform.GetUnitAxis(EAxis::Y) : _owner->GetActorRightVector();
}

FVector UStellarCharacterMotor::GetGlobalUpVector()
{
	return Environement ? FinalTransform.GetUnitAxis(EAxis::Z) : _owner->GetActorUpVector();
}

FVector UStellarCharacterMotor::GetCharacterForwardVector()
{
	return Environement ? LocalOffsetTransform.GetUnitAxis(EAxis::X) : _owner->GetActorForwardVector();
}

FVector UStellarCharacterMotor::GetCharacterRightVector()
{
	return Environement ? LocalOffsetTransform.GetUnitAxis(EAxis::Y) : _owner->GetActorRightVector();
}

FVector UStellarCharacterMotor::GetCharacterUpVector()
{
	return Environement ? LocalOffsetTransform.GetUnitAxis(EAxis::Y) : _owner->GetActorRightVector();
}



void UStellarCharacterMotor::Dev_UpdateMovement(FVector MovementInput)
{
}

bool UStellarCharacterMotor::IsCharacterGrounded()
{
	const FVector Start = Environement ? FinalTransform.GetLocation() : _owner->GetActorLocation();
	const FVector DownVector = -GetGlobalUpVector();
	const FVector End = Start + DownVector * 3.0f; // Use a configurable value instead of _skinWidth * 3

	const float CapsuleRadius = _bounds->GetUnscaledCapsuleRadius();
	const float CapsuleHalfHeight = _bounds->GetUnscaledCapsuleHalfHeight();
	const FQuat CapsuleRotation = Environement ? FinalTransform.GetRotation() : _owner->GetActorQuat();

	FHitResult HitResult = CheckCapsuleCollision(Start, End, CapsuleRadius, CapsuleHalfHeight, CapsuleRotation, true);

	if (HitResult.bBlockingHit)
	{
		// Optionally add slope check:
		const float SlopeDot = FVector::DotProduct(HitResult.Normal, GetGlobalUpVector());
		const float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(SlopeDot, -1.f, 1.f)));

		UE_LOG(LogTemp, Log, TEXT("Ground Hit Angle: %.2f degrees"), SlopeAngle);

		return SlopeAngle <= _maxSlopeAngle; // e.g., 45.f
	}

	return false;
}

FVector UStellarCharacterMotor::CollideAndSlide(FVector vel, FVector initialVel, FVector pos, int depth, bool gravityPass)
{
	if(depth >= _maxBounces) {
		return FVector::ZeroVector;
	}

	
	float dist = vel.Length() + _skinWidth;

	FVector Start = pos;
	FVector End = pos + vel.GetSafeNormal() * dist;
	float CapsuleRadius = _bounds->GetUnscaledCapsuleRadius();
	float CapsuleHalfHeight = _bounds->GetUnscaledCapsuleHalfHeight();
	FQuat CapsuleRotation = _bounds->GetComponentRotation().Quaternion();
	FHitResult HitResult = CheckCapsuleCollision(Start, End, CapsuleRadius, CapsuleHalfHeight, CapsuleRotation, false);

	if(HitResult.bBlockingHit)
	{
		FVector snapToSurface = vel.GetSafeNormal() * (HitResult.Distance - _skinWidth);
		FVector leftOver = vel - snapToSurface;
		
		float Dot = FVector::DotProduct(FVector::UpVector, HitResult.Normal);
		float angleRadians = FMath::Acos(Dot);
		float angleDegree = FMath::RadiansToDegrees(angleRadians);

		
		if(snapToSurface.Length() <= _skinWidth) {
			snapToSurface = FVector::ZeroVector;
		}

		//normal ground or slope
		if(angleDegree <= _maxSlopeAngle) {
			if(gravityPass) {
				return snapToSurface;
			}
			leftOver = ProjectAndScale(leftOver, HitResult.Normal);
		}
		// wall or steep slope
		else
		{
			float scale = 1 - FVector::DotProduct
			(
				FVector(HitResult.Normal.X, HitResult.Normal.Y, 0).GetSafeNormal(),
				-FVector(initialVel.X, initialVel.Y, 0).GetSafeNormal()
			);
			
			if(_isGrounded && !gravityPass)
			{
				leftOver = ProjectAndScale
				(
					FVector(leftOver.X, leftOver.Y, 0),
					FVector(HitResult.Normal.X, HitResult.Normal.Y, 0)
				).GetSafeNormal();
			
				leftOver *= scale;
			}else
			{
				leftOver = ProjectAndScale(leftOver, HitResult.Normal) * scale;
			}
		}

		// float mag = leftOver.Length();
		// leftOver = FVector::VectorPlaneProject(leftOver, HitResult.Normal).GetSafeNormal();
		// leftOver *= mag;
		
		return snapToSurface + CollideAndSlide(leftOver, initialVel, pos + snapToSurface, depth + 1, gravityPass);
	}

	 
	return vel;
}

FVector UStellarCharacterMotor::HomeMadeCollideAndSlide(FVector vel, FVector initialVel, FVector pos, int depth, bool gravityPass)
{
	if(depth >= _maxBounces) {
		return FVector::ZeroVector;
	}
	
	float dist = vel.Length() + _skinWidth;

	FVector Start = pos;
	FVector End = pos + vel.GetSafeNormal() * dist;
	float CapsuleRadius = _bounds->GetUnscaledCapsuleRadius();
	float CapsuleHalfHeight = _bounds->GetUnscaledCapsuleHalfHeight();
	FQuat CapsuleRotation = _bounds->GetComponentRotation().Quaternion();
	FHitResult HitResult = CheckCapsuleCollision(Start, End, CapsuleRadius, CapsuleHalfHeight, CapsuleRotation, false);

	if(HitResult.bBlockingHit)
	{
		FVector snapToSurface = vel.GetSafeNormal() * (HitResult.Distance - _skinWidth);
		FVector velocityLeftOver = vel - snapToSurface;

		if(snapToSurface.Length() <= _skinWidth) {
			snapToSurface = FVector::ZeroVector;
		}
		
		float Dot = FVector::DotProduct(FVector::UpVector, HitResult.Normal);
		float angleRadians = FMath::Acos(Dot);
		float angleDegree = FMath::RadiansToDegrees(angleRadians);


		if(angleDegree <= _maxSlopeAngle) {
			if(gravityPass) {
				return snapToSurface;
			}
			velocityLeftOver = ProjectAndScale(velocityLeftOver, HitResult.Normal);
		}else
		{
			float scale = 1 - FVector::DotProduct
			(
				FVector(HitResult.Normal.X, HitResult.Normal.Y, 0).GetSafeNormal(),
				-FVector(initialVel.X, initialVel.Y, 0).GetSafeNormal()
			);
			
			if(_isGrounded && !gravityPass)
			{
				velocityLeftOver = ProjectAndScale
				(
					FVector(velocityLeftOver.X, velocityLeftOver.Y, 0),
					FVector(HitResult.Normal.X, HitResult.Normal.Y, 0)
				).GetSafeNormal();
			
				velocityLeftOver *= scale;
			}else
			{
				velocityLeftOver = ProjectAndScale(velocityLeftOver, HitResult.Normal) * scale;
			}
		}
		
		return snapToSurface + HomeMadeCollideAndSlide(velocityLeftOver, initialVel, pos + snapToSurface, depth + 1, gravityPass);
	}

	 
	return vel;
}

FHitResult UStellarCharacterMotor::CheckCapsuleCollision(FVector Start, FVector End, float CapsuleRadius,
	float CapsuleHalfHeight, FQuat CapsuleRotation, bool drawDebug)
{
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	
	// Set up collision query parameters
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(_owner); // Ignore self

	// Perform the sweep
	FHitResult HitResult;
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		CapsuleRotation,
		ECC_Visibility, 
		CollisionShape,
		QueryParams
	);

	if(drawDebug)
	{
		// Visualize the sweep path
		FColor TraceColor = bHit ? FColor::Red : FColor::Green;
		float LifeTime = 0.5f; // Seconds

		// Draw capsule at start
		DrawDebugCapsule(GetWorld(), Start, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(),
						 CapsuleRotation, TraceColor, false, LifeTime);

		// Draw capsule at end
		DrawDebugCapsule(GetWorld(), End, CollisionShape.GetCapsuleHalfHeight(), CollisionShape.GetCapsuleRadius(),
						 CapsuleRotation, TraceColor, false, LifeTime);

		// Draw line between start and end
		DrawDebugLine(GetWorld(), Start, End, TraceColor, false, LifeTime, 0, 1.0f);

		// If we hit something, mark the hit location
		if (bHit)
		{
			DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 12.f, FColor::Yellow, false, LifeTime);
			DrawDebugString(GetWorld(), HitResult.ImpactPoint + FVector(0, 0, 20), TEXT("Hit"), nullptr, FColor::White, LifeTime);
		}
	}
	
	return HitResult;
}

FVector UStellarCharacterMotor::ProjectAndScale(FVector vec, FVector normal)
{
	float mag = vec.Length();
	vec = FVector::VectorPlaneProject(vec, normal).GetSafeNormal();
	vec *= mag;
	
	return vec;
}




// Utility function to check if surface is a step
bool UStellarCharacterMotor::IsStepSurface(const FVector& Normal, const FVector& Velocity)
{
	float DotForward = FVector::DotProduct(Normal, -Velocity.GetSafeNormal());
	return DotForward > 0.9f && FMath::Abs(Normal.Z) < 0.05f; // Nearly vertical wall
}

// Try to step up
bool UStellarCharacterMotor::TryStepUp(UWorld* World, const FVector& CurrentPos, const FVector& InputVel, FVector& OutFinalPos)
{
	FHitResult Hit;
	FVector UpOffset = FVector(0, 0, StepHeight);
	FVector ForwardOffset = InputVel.GetSafeNormal() * StepCheckDistance;

	FVector CheckPos = CurrentPos + UpOffset;
	FVector ForwardPos = CheckPos + ForwardOffset;

	// 1. Sweep forward at elevated position
	bool bForwardBlocked = World->SweepTestByChannel(
		CheckPos,
		ForwardPos,
		FQuat::Identity,
		ECC_WorldStatic,
		FCollisionShape::MakeCapsule(25.f, 88.f) // use your capsule dimensions
	);

	if (bForwardBlocked)
	{
		return false; // Blocked by something even after stepping up
	}

	// 2. Sweep down from elevated + forward position
	FVector DownStart = ForwardPos;
	FVector DownEnd = ForwardPos - UpOffset;

	bool bFoundGround = World->SweepSingleByChannel(
		Hit,
		DownStart,
		DownEnd,
		FQuat::Identity,
		ECC_WorldStatic,
		FCollisionShape::MakeCapsule(25.f, 88.f)
	);

	if (!bFoundGround || Hit.bStartPenetrating)
	{
		return false;
	}

	OutFinalPos = Hit.Location;
	return true;
}