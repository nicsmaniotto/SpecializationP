// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <Enums.h>
#include "EnergyStructure.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEnergyEnd, EEnergyType, EnergyType);

UDELEGATE()
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnUpdate, EEnergyType, EnergyType, float, OldValue, float, NewValue);

/**
 *
 */
UCLASS()
class SPECIALIZATION_API UEnergyStructure : public UObject
{
	GENERATED_BODY()

public:
	void Init(EEnergyType EnergyType, float _GrowthValue, float _HardGrowthValue, float _DecayValue, float _HardDecayValue);

	void Update(float DeltaSeconds);

	bool StartConsumeEnergy(bool IsHardValue = false);

	void StopConsumeEnergy();
	
	void ToggleRestoreEnergy(bool Active);

	float GetEnergy() const { return Energy; }

	EEnergyType GetEnergyType() const { return Type; }

	bool IsConsumingEnergy() const { return TargetEnergy == 0; }

	bool HasEnergy() const { return Energy > 0; }

	UPROPERTY()
	FOnEnergyEnd OnEnergyEnd;
	
	UPROPERTY()
	FOnUpdate OnUpdate;

private:
	EEnergyType Type;

	float Energy = 1;

	float TargetEnergy = 1;

	float GrowthValue;
	
	float DecayValue;
	
	float HardGrowthValue;
	
	float HardDecayValue;

	bool IsHardUpdating;
};
