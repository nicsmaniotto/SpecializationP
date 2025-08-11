// Fill out your copyright notice in the Description page of Project Settings.


#include "Jetpack.h"
#include "EnergyComponent.h"
#include <Kismet/KismetMathLibrary.h>

void UJetpack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandlePropSpeed(DeltaTime);
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

void UJetpack::Throttle(const FInputActionValue& Value)
{
	if (GravityForce != FVector::ZeroVector)
	{
		if (!HasPropulsion())
		{
			EnergyComponent->StopConsumeEnergy(FuelConsumptionType);
			return;
		}
	}

	FuelConsumptionType = EnergyComponent->StartConsumeEnergy(FuelConsumptionMap);

	if (FuelConsumptionType != EEnergyType::NONE)
	{
		Super::Throttle(Value);
	}
}

void UJetpack::EndThrottle(const FInputActionValue& Value)
{
	if (FuelConsumptionType != EEnergyType::NONE)
	{
		EnergyComponent->StopConsumeEnergy(FuelConsumptionType);
	}

	Super::EndThrottle(Value);
}

void UJetpack::Move(const FInputActionValue& Value)
{
	FuelConsumptionType = EnergyComponent->StartConsumeEnergy(FuelConsumptionMap);

	if (FuelConsumptionType != EEnergyType::NONE)
	{
		Super::Move(Value);
	}
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

void UJetpack::AskReposition(ERepositionType RepositionType, FVector RepositionTorqueForce, bool ForceReposition)
{
	//if (!OnAir || !OwnerPhysicsComponent->IsSimulatingPhysics()) return;
	if (!OwnerPhysicsComponent->IsSimulatingPhysics()) return;

	Super::AskReposition(RepositionType, RepositionTorqueForce, ForceReposition);


}
