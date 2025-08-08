// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyStructure.h"
#include <Kismet/KismetMathLibrary.h>

void UEnergyStructure::Init(
	EEnergyType EnergyType,
	float _GrowthValue,
	float _DecayValue,
	float _HardGrowthValue,
	float _HardDecayValue)
{
	Type = EnergyType;
	GrowthValue = _GrowthValue;
	DecayValue = _DecayValue;
	HardGrowthValue = _HardGrowthValue;
	HardDecayValue = _HardDecayValue;

	Energy = 1;
	TargetEnergy = 1;
}

void UEnergyStructure::Update(float DeltaSeconds)
{
	float UpdateSpeed = IsHardUpdating ? HardGrowthValue : GrowthValue;

	if (TargetEnergy == 0)
	{
		UpdateSpeed = IsHardUpdating ? HardDecayValue : DecayValue;
	}

	float OldEnergy = Energy;
	Energy = UKismetMathLibrary::FInterpTo_Constant(Energy, TargetEnergy, DeltaSeconds, UpdateSpeed);

	if (Type != EEnergyType::FUEL) GEngine->AddOnScreenDebugMessage(-1, .01f, FColor::Emerald, FString::Printf(TEXT("Velocity: %f - Value: %f"), UpdateSpeed, Energy));

	if (Energy > 0)
	{
		OnUpdate.ExecuteIfBound(Type, OldEnergy, Energy);
	}
	else
	{
		if (TargetEnergy == 0)
		{
			OnUpdate.ExecuteIfBound(Type, OldEnergy, Energy);

			OnEnergyEnd.ExecuteIfBound(Type);

			IsHardUpdating = false;
		}
	}
}

bool UEnergyStructure::StartConsumeEnergy(bool IsHardValue)
{
	if (Energy == 0) return false;
	if (IsHardUpdating && TargetEnergy == 1) return true;
	if (!IsHardUpdating)
	{
		IsHardUpdating = IsHardValue;
	}

	TargetEnergy = 0;

	return true;
}

void UEnergyStructure::StopConsumeEnergy()
{
	if (IsHardUpdating && TargetEnergy == 1) return;

	IsHardUpdating = false;

	TargetEnergy = 1;
}

void UEnergyStructure::ToggleRestoreEnergy(bool Active)
{
	IsHardUpdating = Active;

	TargetEnergy = 1;
}
