// Fill out your copyright notice in the Description page of Project Settings.


#include "Jetpack.h"
#include "EnergyComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "Specialization/SpecializationCharacter.h"
#include "Camera/CameraComponent.h"
#include "Planet.h"
#include <InputActionValue.h>

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

		FVector DampCompensation = OwnerPhysicsComponent->GetPhysicsLinearVelocity() / OwnerPhysicsComponent->GetLinearDamping();
		FVector PlanetForces = PlanetSurface->GetDeltaVelocity(OwnerLoc) + PlanetSurface->GetDeltaAngForce(OwnerLoc);

		OwnerPhysicsComponent->AddForce(
			(PlanetSurface->GetDeltaVelocity(OwnerLoc) + PlanetSurface->GetDeltaAngForce(OwnerLoc)) * OwnerPhysicsComponent->GetLinearDamping(),
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
		PlanetVelocity = PlanetSurface->GetDeltaVelocity(OwnerPhysicsComponent->GetComponentLocation()) + PlanetSurface->GetDeltaAngForce(OwnerPhysicsComponent->GetComponentLocation());
	}

	FVector ForVector = OwnerPhysicsComponent->GetForwardVector();

	FVector RightVector = OwnerPhysicsComponent->GetRightVector();

	if (GetGForce().SquaredLength() > 0)
	{
		ForVector = FVector::VectorPlaneProject(ForVector, -GetGForce().GetSafeNormal()).GetSafeNormal();
		RightVector = FVector::CrossProduct(-GetGForce().GetSafeNormal(), ForVector);
	}

	LastMovDirection = (ForVector * MovementVector.Y + RightVector * MovementVector.X).GetSafeNormal();

	ForVector = LastMovDirection * MaxWalkSpeed;

	if (!GetOnAir())
	{
		FVector SurfaceNormal = GetHitSurface().ImpactNormal;
		ForVector = FVector::CrossProduct(ForVector, SurfaceNormal);
		ForVector = FVector::CrossProduct(SurfaceNormal, ForVector);

		ForVector += PlanetVelocity;

		OwnerPhysicsComponent->SetPhysicsLinearVelocity(ForVector, false);

		MovType = EMovType::WALK;
	}
	else
	{
		if (!!PlanetSurface) OwnerPhysicsComponent->AddForce(ForVector * OwnerPhysicsComponent->GetLinearDamping(), NAME_None, true);

		MovType = EMovType::NONE;
	}

	OnMovement.Broadcast(MovType);
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
	//if (!OwnerPhysicsComponent->IsSimulatingPhysics()) return;
	IsForcedReposition = ForceReposition;

	//if (IsForcedReposition) Super::AskReposition_Implementation(RepositionType, RepositionTorqueForce, ForceReposition);
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
	USceneComponent* Repositionable = IRepositionable::Execute_GetRepositionableComponent(this);
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
				(PlanetSurface->GetDeltaVelocity(OwnerLoc) + PlanetSurface->GetDeltaAngForce(OwnerLoc)) * (OwnerPhysicsComponent->GetLinearDamping()),
				NAME_None, true);
		}
		else if (MovType != EMovType::WALK && !bHasJumped)
		{
			OwnerPhysicsComponent->SetPhysicsLinearVelocity(PlanetSurface->GetDeltaVelocity(OwnerLoc) + PlanetSurface->GetDeltaAngForce(OwnerLoc), false);
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

	FVector JumpDir = OwnerPhysicsComponent->GetUpVector();
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
