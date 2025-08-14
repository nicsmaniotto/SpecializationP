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

	/*if (!OnAir)
	{
		FHitResult Hit;
		FVector Loc = UKismetMathLibrary::TransformLocation(GetOwner()->GetTransform(), FeetPosition);

		GetWorld()->LineTraceSingleByChannel(Hit, Loc, Loc - OwnerPhysicsComponent->GetUpVector() * AirCheckRadius, ECollisionChannel::ECC_Visibility);

		if (Hit.bBlockingHit)
		{
			FVector LookV = FVector::VectorPlaneProject(OwnerPhysicsComponent->GetForwardVector(), Hit.ImpactNormal);
			FRotator r = UKismetMathLibrary::FindLookAtRotation(Loc, Loc + LookV);

			DrawDebugLine(GetWorld(), Loc, Loc + LookV * 300, FColor::Red, false, .1f);

			r = UKismetMathLibrary::RLerp(OwnerPhysicsComponent->GetComponentRotation(), r, DeltaTime * .1f, true);


			Camera->SetWorldRotation(r.Quaternion(), false, nullptr, ETeleportType::TeleportPhysics);
		}
	}
	else
	{
	}*/

	/*USceneComponent* Camera = Cast<ASpecializationCharacter>(GetOwner())->GetFirstPersonCameraComponent();

	FVector LookV = FVector::VectorPlaneProject(OwnerPhysicsComponent->GetForwardVector(), -GravityForce.GetSafeNormal());
	FVector RightVec = -FVector::CrossProduct(LookV, -GravityForce.GetSafeNormal());


	FRotator r = UKismetMathLibrary::FindLookAtRotation(OwnerPhysicsComponent->GetComponentLocation(), OwnerPhysicsComponent->GetComponentLocation() + LookV);

	Camera->SetWorldRotation(r.Quaternion(), false, nullptr, ETeleportType::TeleportPhysics);*/
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
	if (!OwnerPhysicsComponent->IsSimulatingPhysics()) return;
	//if (!OwnerPhysicsComponent->IsSimulatingPhysics()) return;

	//GetRepositionableComponent()->SetWorldRotation(r.Quaternion(), false, nullptr, ETeleportType::TeleportPhysics);

	FVector RepositionForce = RepositionTorqueForce.GetSafeNormal();
	FQuat Q = FQuat(RepositionForce, GetWorld()->GetDeltaSeconds());

	IRepositionable::Execute_GetRepositionableComponent(this)->AddWorldRotation(Q.Rotator(), false, nullptr, ETeleportType::TeleportPhysics);
}

USceneComponent* UJetpack::GetRepositionableComponent_Implementation() const
{
	if (!GetOwner()->GetClass()->ImplementsInterface(URepositionable::StaticClass())) return Super::GetRepositionableComponent_Implementation();

	USceneComponent* Repositionable = IRepositionable::Execute_GetRepositionableComponent(GetOwner());

	if (!Repositionable) return Super::GetRepositionableComponent_Implementation();

	return Repositionable;
}
