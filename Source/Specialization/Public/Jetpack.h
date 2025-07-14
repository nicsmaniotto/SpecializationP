// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireEngine.h"
#include "Jetpack.generated.h"

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


public:
	void Throttle(const FInputActionValue& Value) override;

	void UpdateGravityForce(FVector OldGForce, FVector NewGForce) override;

};
