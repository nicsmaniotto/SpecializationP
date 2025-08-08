// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enums.h"
#include "EnergyComponent.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEnergyUpdate, EEnergyType, EnergyType, float, OldValue, float, NewValue);

class UEnergyStructure;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPECIALIZATION_API UEnergyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnergyComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Energy")
	TArray<FEnergyInfo> EnergyInfos;
	
	UPROPERTY(BlueprintReadOnly, Category = "Energy")
	TArray<UEnergyStructure*> EnergyStructures;

	UFUNCTION()
	void OnEnergyUpdated(EEnergyType EnergyType, float OldValue, float NewValue);

public:
	UFUNCTION(BlueprintCallable)
	EEnergyType StartConsumeEnergy(TMap<EEnergyType, bool> EnergyPriorityTypes);
	
	UFUNCTION(BlueprintCallable)
	void StopConsumeEnergy(EEnergyType ConsumingEnergyType);
	
	UFUNCTION(BlueprintCallable)
	void RestoreMaxEnergy(EEnergyType ConsumingEnergyType, bool Active);

	UPROPERTY(BlueprintAssignable)
	FOnEnergyUpdate OnEnergyUpdate;

	UFUNCTION(BlueprintCallable)
	bool HasEnergyType(EEnergyType Type) const;
};
