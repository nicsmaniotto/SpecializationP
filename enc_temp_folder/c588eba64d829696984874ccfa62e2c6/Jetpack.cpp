// Fill out your copyright notice in the Description page of Project Settings.


#include "Jetpack.h"
#include "EnergyComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "Specialization/SpecializationCharacter.h"
#include "Camera/CameraComponent.h"
#include "Planet.h"
#include <InputActionValue.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Components/CapsuleComponent.h>

void UJetpack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandlePropSpeed(DeltaTime);

	CustomReposition(DeltaTime);

	PositionAdjustment(DeltaTime);

	JumpTick(DeltaTime);
}

void UJetpack::HandlePropSpeed(float DeltaTime)
{
	float PropSpeed = AirPropulsionGrowthSpeed;
	float TargetValue = 1;

	if (!OnAir)
	{
		PropSpeed = GroundPropulsionGrowthSpeed;
	}
	else
	{
		if (IsThrottling)
		{
			PropSpeed = PropulsionDecaySpeed;
			TargetValue = 0;
		}
	}

	PropulsionValue = UKismetMathLibrary::FInterpTo_Constant(PropulsionValue, TargetValue, DeltaTime, PropSpeed);
}

FRotator UJetpack::GetDirectionRotation() const
{
	FVector LookV = FVector::VectorPlaneProject(OwnerPhysicsComponent->GetForwardVector(), -GravityForce.GetSafeNormal()).GetSafeNormal();
	FVector RightVec = -FVector::CrossProduct(LookV, -GravityForce.GetSafeNormal());
	//FVector UpCorrected = FVector::CrossProduct(LookV, RightVec);

	FMatrix LookAtMatrix;
	LookAtMatrix.SetAxis(0, LookV);
	LookAtMatrix.SetAxis(1, RightVec);
	LookAtMatrix.SetAxis(2, -GravityForce.GetSafeNormal());

	return LookAtMatrix.Rotator();
}

void UJetpack::SetDependencyComponent(UMarker* MarkerComponent, UEnergyComponent* _EnergyComponent)
{
	Super::SetDependencyComponent(MarkerComponent);

	EnergyComponent = _EnergyComponent;
}

void UJetpack::StartMove(const FInputActionValue& Value)
{
	Super::StartMove(Value);

	APlanet* PlanetSurface = GetPlanetSurface();

	if (PlanetSurface && !GetOnAir())
	{
		FVector OwnerLoc = OwnerPhysicsComponent->GetComponentLocation();

		OwnerPhysicsComponent->AddForce(
			PlanetSurface->GetDeltaVelocity(OwnerLoc) * OwnerPhysicsComponent->GetLinearDamping(),
			NAME_None, true);
	}
}

void UJetpack::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	//if (bOnJetpack && Jetpack->GetOnAir())
	if (GetOnAir())
	{
		MovType = EMovType::AIR;

		OnMovement.Broadcast(MovType);

		Super::Move(Value);
		return;
	}

	MovementVector.Normalize();

	// add movement 
	FVector PlanetVelocity = FVector::ZeroVector;

	APlanet* PlanetSurface = GetPlanetSurface();
	if (PlanetSurface)
	{
		PlanetVelocity = PlanetSurface->GetDeltaVelocity(OwnerPhysicsComponent->GetComponentLocation());
	}

	USceneComponent* Repositionable = IRepositionable::Execute_GetRepositionableComponent(this);

	FVector ForVector = Repositionable->GetForwardVector();

	FVector RightVector = Repositionable->GetRightVector();

	if (GetGForce().SquaredLength() > 0)
	{
		ForVector = FVector::VectorPlaneProject(ForVector, -GetGForce().GetSafeNormal()).GetSafeNormal();
		RightVector = FVector::CrossProduct(-GetGForce().GetSafeNormal(), ForVector);
	}

	LastMovDirection = (ForVector * MovementVector.Y + RightVector * MovementVector.X).GetSafeNormal();

	if (!WalkMoveCheck(LastMovDirection)) return;

	if (!GetOnAir())
	{
		FVector PlaneNormal = FVector::CrossProduct(LastMovDirection, GetGForce().GetSafeNormal());
		FVector SurfaceNormal = FVector::VectorPlaneProject(GetHitSurface().ImpactNormal, PlaneNormal).GetSafeNormal();

		LastMovDirection = FVector::CrossProduct(LastMovDirection, SurfaceNormal);
		LastMovDirection = FVector::CrossProduct(SurfaceNormal, LastMovDirection);

		FVector ImpressForce = LastMovDirection * MaxWalkSpeed;

		DrawDebugLine(GetWorld(), OwnerPhysicsComponent->GetComponentLocation(), OwnerPhysicsComponent->GetComponentLocation() + ImpressForce, FColor::Red, false, .1f);

		if (LandingPlanet)
		{
			ImpressForce = ImpressForce.RotateAngleAxis(LandingPlanet->GetDeltaAngForce().Length(), LandingPlanet->GetDeltaAngForce());
		}

		DrawDebugLine(GetWorld(), OwnerPhysicsComponent->GetComponentLocation(), OwnerPhysicsComponent->GetComponentLocation() + ImpressForce, FColor::Emerald, false, .1f);

		//OwnerPhysicsComponent->SetPhysicsLinearVelocity(LastMovDirection * MaxWalkSpeed + PlanetVelocity, false);
		OwnerPhysicsComponent->SetPhysicsLinearVelocity(ImpressForce + PlanetVelocity, false);

		MovType = EMovType::WALK;
	}
	else
	{
		if (!!PlanetSurface) OwnerPhysicsComponent->AddForce(LastMovDirection * MaxWalkSpeed * OwnerPhysicsComponent->GetLinearDamping(), NAME_None, true);

		MovType = EMovType::NONE;
	}

	OnMovement.Broadcast(MovType);
}

bool UJetpack::WalkMoveCheck(FVector& MovementDir)
{
	UCapsuleComponent* OwnerCapsule = Cast<UCapsuleComponent>(OwnerPhysicsComponent);

	if (!OwnerCapsule) return true;

	FVector Start = OwnerPhysicsComponent->GetComponentLocation() + OwnerPhysicsComponent->GetUpVector() * OwnerCapsule->GetScaledCapsuleHalfHeight();

	TArray<UPrimitiveComponent*> Out;

	FHitResult Hit;

	UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, Start + StopWalkCheckLength * MovementDir * MaxWalkSpeed * GetWorld()->GetDeltaSeconds(),
		OwnerCapsule->GetScaledCapsuleRadius() + StopWalkCheckOffset , StopWalkTypesCheck, false, { GetOwner() }, EDrawDebugTrace::None, Hit, true);

	if (Hit.bBlockingHit)
	{
		if (FVector::DotProduct(MovementDir, Hit.ImpactNormal) > 0) return true;

		FVector ImpactNormal = Hit.ImpactNormal;

		FVector NormalAdjusted = FVector::VectorPlaneProject(ImpactNormal, GetGForce().GetSafeNormal()).GetSafeNormal();
		
		if (FVector::DotProduct(FVector::VectorPlaneProject(MovementDir, GetGForce().GetSafeNormal()).GetSafeNormal(), NormalAdjusted) < -.8f)
		{
			return false;
		}

		MovementDir = FVector::CrossProduct(MovementDir, NormalAdjusted);
		MovementDir = FVector::CrossProduct(NormalAdjusted, MovementDir);
	}

	return true;
}

void UJetpack::StopMove(const FInputActionValue& Value)
{
	MovType = EMovType::NONE;

	LastMovDirection = FVector::ZeroVector;

	if (FuelConsumptionType != EEnergyType::NONE)
	{
		EnergyComponent->StopConsumeEnergy(FuelConsumptionType);
	}

	Super::StopMove(Value);
}


bool UJetpack::VerticalMovement(float GravityMultiplier)
{
	if (GravityForce != FVector::ZeroVector)
	{
		if (!HasPropulsion())
		{
			EnergyComponent->StopConsumeEnergy(FuelConsumptionType);
			return false;
		}
	}

	FuelConsumptionType = EnergyComponent->StartConsumeEnergy(FuelConsumptionMap);

	if (FuelConsumptionType != EEnergyType::NONE)
	{
		return Super::VerticalMovement(GravityMultiplier);
	}

	return false;
}

void UJetpack::EndThrottle(const FInputActionValue& Value)
{
	if (FuelConsumptionType != EEnergyType::NONE)
	{
		EnergyComponent->StopConsumeEnergy(FuelConsumptionType);
	}

	Super::EndThrottle(Value);
}

bool UJetpack::HorizontalMovement(FVector2D LookAxisVector)
{
	FuelConsumptionType = EnergyComponent->StartConsumeEnergy(FuelConsumptionMap);

	if (FuelConsumptionType != EEnergyType::NONE)
	{
		return Super::HorizontalMovement(LookAxisVector);
	}

	return false;
}

void UJetpack::Reverse(const FInputActionValue& Value)
{
	FuelConsumptionType = EnergyComponent->StartConsumeEnergy(FuelConsumptionMap);

	if (FuelConsumptionType != EEnergyType::NONE)
	{
		Super::Reverse(Value);
	}
}

void UJetpack::StopReverse(const FInputActionValue& Value)
{
	if (FuelConsumptionType != EEnergyType::NONE)
	{
		EnergyComponent->StopConsumeEnergy(FuelConsumptionType);
	}

	Super::StopReverse(Value);
}

void UJetpack::UpdateGravityForce(FVector OldGForce, FVector NewGForce)
{
	Super::UpdateGravityForce(OldGForce, NewGForce);

	if (GravityForce == FVector::ZeroVector)
	{
		// notify propulsion not needed
	}
}

void UJetpack::AskReposition_Implementation(ERepositionType RepositionType, FVector RepositionTorqueForce, bool ForceReposition)
{
	if (!OnAir || !OwnerPhysicsComponent->IsSimulatingPhysics()) return;

	IsForcedReposition = ForceReposition;

	Super::AskReposition_Implementation(RepositionType, RepositionTorqueForce, ForceReposition);
}

USceneComponent* UJetpack::GetRepositionableComponent_Implementation() const
{
	if (!GetOwner()->GetClass()->ImplementsInterface(URepositionable::StaticClass()) || IsForcedReposition) return Super::GetRepositionableComponent_Implementation();

	USceneComponent* Repositionable = IRepositionable::Execute_GetRepositionableComponent(GetOwner());

	if (!Repositionable) return Super::GetRepositionableComponent_Implementation();

	return Repositionable;
}

void UJetpack::CustomReposition(float DeltaTime)
{
	// Physics reposition
	APlanet* PlanetSurface = GetPlanetSurface();

	if (PlanetSurface && !GetOnAir())
	{
		OwnerPhysicsComponent->AddTorqueInDegrees(PlanetSurface->GetDeltaAngForce() * OwnerPhysicsComponent->GetAngularDamping(), NAME_None, true);
	}

	// Repositionable reposition
	USceneComponent* Repositionable = IRepositionable::Execute_GetRepositionableComponent(GetOwner());
	FRotator NextRotation;

	if (GravityForce.SquaredLength() == 0 || IsForcedReposition)
	{
		NextRotation = OwnerPhysicsComponent->GetComponentRotation();
	}
	else
	{
		NextRotation = GetDirectionRotation();
	}

	FRotator r = UKismetMathLibrary::RLerp(Repositionable->GetComponentRotation(), NextRotation, DeltaTime * CustomRepositionLerpSpeed, true);

	Repositionable->SetWorldRotation(r, false, nullptr, ETeleportType::TeleportPhysics);
}

void UJetpack::PositionAdjustment(float DeltaTime)
{
	APlanet* PlanetSurface = GetPlanetSurface();

	if (PlanetSurface)
	{
		FVector OwnerLoc = OwnerPhysicsComponent->GetComponentLocation();
		if (GetOnAir())
		{
			OwnerPhysicsComponent->AddForce(
				PlanetSurface->GetDeltaVelocity(OwnerLoc) * (OwnerPhysicsComponent->GetLinearDamping()),
				NAME_None, true);
		}
		else if (MovType != EMovType::WALK && !bHasJumped)
		{
			OwnerPhysicsComponent->SetPhysicsLinearVelocity(PlanetSurface->GetDeltaVelocity(OwnerLoc), false);
		}
	}
}

void UJetpack::JumpTick(float DeltaTime)
{
	if (bHasJumped && GetOnAir())
	{
		if (GetGForce().SquaredLength() > 0
			&& FVector::DotProduct(GetGForce(), OwnerPhysicsComponent->GetPhysicsLinearVelocity()) > 0)
		{
			if (JumpTopFreedomTimer < JumpTopFreedomTime)
			{
				JumpTopFreedomTimer += DeltaTime;
				OwnerPhysicsComponent->AddForce(-GetGForce() / JumpTopGravityDivider, NAME_None, true);
			}
			else
			{
				bHasJumped = false;
				JumpTopFreedomTimer = 0;
			}
		}
	}
}

bool UJetpack::Jump()
{
	if (!JumpForceCurve || GetOnAir()) return false;

	JumpHoldTimer = 0;

	return true;
}

bool UJetpack::JumpOnGoing()
{
	if (!JumpForceCurve || GetOnAir()) return false;

	JumpHoldTimer += GetWorld()->GetDeltaSeconds();

	return true;
}

bool UJetpack::StopJumping()
{
	if (!JumpForceCurve) return false;
	if (JumpHoldTimer <= 0) return false;
	if (GetOnAir()) return false;

	USceneComponent* Repositionable = IRepositionable::Execute_GetRepositionableComponent(this);

	FVector JumpDir = Repositionable->GetUpVector();
	if (GetGForce().SquaredLength() > 0)
	{
		JumpDir += LastMovDirection * JumpDirectionMultiplier;
	}

	float ForceToApply = JumpForceCurve->GetFloatValue(JumpHoldTimer);

	OwnerPhysicsComponent->AddImpulse(JumpDir.GetSafeNormal() * ForceToApply, NAME_None, true);

	bHasJumped = true;

	return true;
}

bool UJetpack::ToggleJetpack()
{
	bOnJetpack = !bOnJetpack;

	OnJetpackEquip.Broadcast(bOnJetpack);

	return bOnJetpack;
}
