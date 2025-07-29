// Fill out your copyright notice in the Description page of Project Settings.


#include "Jetpack.h"
#include <Kismet/KismetMathLibrary.h>

void UJetpack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandlePropSpeed(DeltaTime);

	/*if (GravityForce != FVector::ZeroVector)
	{
		FVector GravityDir = -GravityForce;
		GravityDir.Normalize();
		FVector NewFor = FVector::VectorPlaneProject(GetOwner()->GetActorRightVector(), GravityDir).RotateAngleAxis(-90, GravityDir);

		FRotator r = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + NewFor);
		r = UKismetMathLibrary::RLerp(GetOwner()->GetActorRotation(), r, 20 * DeltaTime, true);

		DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + NewFor * 200, FColor::Red, false, .1f);
		GetOwner()->SetActorRotation(r.Quaternion(), ETeleportType::TeleportPhysics);
	}*/
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

void UJetpack::AskReposition(ERepositionType RepositionType, FVector RepositionTorqueForce, bool ForceReposition)
{
	//if (!OnAir || !OwnerPhysicsComponent->IsSimulatingPhysics()) return;
	if (!OwnerPhysicsComponent->IsSimulatingPhysics()) return;

	Super::AskReposition(RepositionType, RepositionTorqueForce, ForceReposition);


}
