// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireEngine.h"
#include "Enums.h"
#include "Jetpack.generated.h"

class UEnergyComponent;

/**
 *
 */
UCLASS()
class SPECIALIZATION_API UJetpack : public UFireEngine
{
	GENERATED_BODY()

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Propulsion")
	float AirPropulsionGrowthSpeed = .8f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Propulsion")
	float GroundPropulsionGrowthSpeed = 2;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Propulsion")
	float PropulsionDecaySpeed = 1.2f;

	UFUNCTION()
	void HandlePropSpeed(float DeltaTime);

	float PropulsionValue = 1;

	bool bCanThrottle = true;

	UPROPERTY(BlueprintReadOnly, Category = "Energy")
	UEnergyComponent* EnergyComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Energy")
	TMap<EEnergyType, bool> FuelConsumptionMap;

	EEnergyType FuelConsumptionType;

public:
	void SetDependencyComponent(UMarker* MarkerComponent, UEnergyComponent* _EnergyComponent);

	void Throttle(const FInputActionValue& Value) override;
	
	void EndThrottle(const FInputActionValue& Value) override;
	
	void Move(const FInputActionValue& Value) override;
	
	void StopMove(const FInputActionValue& Value) override;
	
	void Reverse(const FInputActionValue& Value) override;
	
	void StopReverse(const FInputActionValue& Value) override;

	void UpdateGravityForce(FVector OldGForce, FVector NewGForce) override;

	virtual void AskReposition(ERepositionType RepositionType, FVector RepositionTorqueForce, bool ForceReposition = false) override;

	bool HasPropulsion() const { return PropulsionValue > 0; }

	UFUNCTION(BlueprintCallable)
	float GetPropulsionValue() const { return PropulsionValue; }
};
