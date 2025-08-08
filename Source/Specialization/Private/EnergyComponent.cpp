// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyComponent.h"
#include "EnergyStructure.h"

// Sets default values for this component's properties
UEnergyComponent::UEnergyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnergyComponent::BeginPlay()
{
	Super::BeginPlay();

	for (FEnergyInfo& I : EnergyInfos)
	{
		UEnergyStructure* ES = NewObject<UEnergyStructure>(this, UEnergyStructure::StaticClass());

		if (ES)
		{
			ES->Init(I.EnergyType, I.GrowthValue, I.DecayValue, I.HardGrowthValue, I.HardDecayValue);
			ES->OnEnergyEnd.BindDynamic(this, &UEnergyComponent::StopConsumeEnergy);
			ES->OnUpdate.BindDynamic(this, &UEnergyComponent::OnEnergyUpdated);

			EnergyStructures.Add(ES);
		}
	}

}


// Called every frame
void UEnergyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (UEnergyStructure* ES : EnergyStructures)
	{
		ES->Update(DeltaTime);
	}
}

void UEnergyComponent::OnEnergyUpdated(EEnergyType EnergyType, float OldValue, float NewValue)
{
	OnEnergyUpdate.Broadcast(EnergyType, OldValue, NewValue);
}

EEnergyType UEnergyComponent::StartConsumeEnergy(TMap<EEnergyType, bool> EnergyPriorityTypes)
{
	for (auto& KeyValue : EnergyPriorityTypes)
	{
		UEnergyStructure** RetrievedItem = EnergyStructures.FindByPredicate([&](UEnergyStructure* Item)
			{
				return Item->GetEnergyType() == KeyValue.Key && Item->HasEnergy();
			});

		if (RetrievedItem)
		{
			if (!(*RetrievedItem)->StartConsumeEnergy(KeyValue.Value))
			{
				return EEnergyType::NONE;
			}

			return (*RetrievedItem)->GetEnergyType();
		}
	}

	return EEnergyType::NONE;
}

void UEnergyComponent::StopConsumeEnergy(EEnergyType ConsumingEnergyType)
{
	for (UEnergyStructure* ES : EnergyStructures)
	{
		if (ES->GetEnergyType() == ConsumingEnergyType)
		{
			ES->StopConsumeEnergy();
			break;
		}
	}
}

void UEnergyComponent::RestoreMaxEnergy(EEnergyType ConsumingEnergyType, bool Active)
{
	for (UEnergyStructure* ES : EnergyStructures)
	{
		if (ES->GetEnergyType() == ConsumingEnergyType)
		{
			ES->ToggleRestoreEnergy(Active);
			break;
		}
	}
}

bool UEnergyComponent::HasEnergyType(EEnergyType Type) const
{
	return EnergyStructures.ContainsByPredicate([&](UEnergyStructure* Item)
		{
			return Item->GetEnergyType() == Type;
		});
}

