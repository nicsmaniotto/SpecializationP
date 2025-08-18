// Fill out your copyright notice in the Description page of Project Settings.


#include "Jetpack.h"
#include "EnergyComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "Specialization/SpecializationCharacter.h"
#include "Camera/CameraComponent.h"

void UJetpack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandlePropSpeed(DeltaTime);

	CustomReposition(DeltaTime);
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

void UJetpack::SetDependencyComponent(UMarker* MarkerComponent, UEnergyComponent* _EnergyComponent)
{
	Super::SetDependencyComponent(MarkerComponent);

	EnergyComponent = _EnergyComponent;
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

void UJetpack::StopMove(const FInputActionValue& Value)
{
	if (FuelConsumptionType != EEnergyType::NONE)
	{
		EnergyComponent->StopConsumeEnergy(FuelConsumptionType);
	}

	Super::StopMove(Value);
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

	if(IsForcedReposition) Super::AskReposition_Implementation(RepositionType, RepositionTorqueForce, ForceReposition);
}

USceneComponent* UJetpack::GetRepositionableComponent_Implementation() const
{
	if (!GetOwner()->GetClass()->ImplementsInterface(URepositionable::StaticClass())) return Super::GetRepositionableComponent_Implementation();

	USceneComponent* Repositionable = IRepositionable::Execute_GetRepositionableComponent(GetOwner());

	if (!Repositionable) return Super::GetRepositionableComponent_Implementation();

	return Repositionable;
}

void UJetpack::CustomReposition(float DeltaTime)
{
	if (GravityForce.SquaredLength() == 0 || IsForcedReposition) return;

	USceneComponent* Repositionable = IRepositionable::Execute_GetRepositionableComponent(this);

	FVector LookV = FVector::VectorPlaneProject(OwnerPhysicsComponent->GetForwardVector(), -GravityForce.GetSafeNormal()).GetSafeNormal();
	FVector RightVec = -FVector::CrossProduct(LookV, -GravityForce.GetSafeNormal());
	//FVector UpCorrected = FVector::CrossProduct(LookV, RightVec);

	FMatrix LookAtMatrix;
	LookAtMatrix.SetAxis(0, LookV);
	LookAtMatrix.SetAxis(1, RightVec);
	LookAtMatrix.SetAxis(2, -GravityForce.GetSafeNormal());


	FRotator r = UKismetMathLibrary::RLerp(Repositionable->GetComponentRotation(), LookAtMatrix.Rotator(), DeltaTime * 50, true);

	DrawDebugLine(GetWorld(), Repositionable->GetComponentLocation(), Repositionable->GetComponentLocation() + LookV * 100, FColor::Emerald, false, .1f);
	DrawDebugLine(GetWorld(), Repositionable->GetComponentLocation(), Repositionable->GetComponentLocation() + RightVec * 100, FColor::Red, false, .1f);

	Repositionable->SetWorldRotation(r, false, nullptr, ETeleportType::TeleportPhysics);
}
