// Fill out your copyright notice in the Description page of Project Settings.


#include "Jetpack.h"
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

void UJetpack::Throttle(const FInputActionValue& Value)
{
	if (GravityForce != FVector::ZeroVector)
	{
		if (PropulsionValue <= 0) return;
	}

	Super::Throttle(Value);
}

void UJetpack::UpdateGravityForce(FVector OldGForce, FVector NewGForce)
{
	Super::UpdateGravityForce(OldGForce, NewGForce);

	if (GravityForce == FVector::ZeroVector)
	{
		// notify propulsion not needed
	}
}

void UJetpack::AskReposition(FVector RepositionTorqueForce, bool ForceReposition)
{
	if (!OnAir) return;

	Super::AskReposition(RepositionTorqueForce, ForceReposition);
}
