// Fill out your copyright notice in the Description page of Project Settings.


#include "FireEngine.h"
#include <Kismet/KismetMathLibrary.h>
#include <InputActionValue.h>
#include <Kismet/KismetSystemLibrary.h>

// Sets default values for this component's properties
UFireEngine::UFireEngine()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFireEngine::BeginPlay()
{
	Super::BeginPlay();

	OwnerMesh = GetOwner()->GetComponentByClass<UMeshComponent>();

}


// Called every frame
void UFireEngine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AdjustDirection();

	OnAir = AirChecker();

	if (!(OnAir || IsThrottling))
	{
		ThrottleCurveEvaluation = 0;
	}
}

bool UFireEngine::AirChecker()
{
	TArray<UPrimitiveComponent*> Hits;

	UKismetSystemLibrary::SphereOverlapComponents(GetWorld(), OwnerMesh->GetComponentLocation(), AirCheckRadius, { EObjectTypeQuery::ObjectTypeQuery1 },
		UStaticMeshComponent::StaticClass(), { GetOwner() }, Hits);

	//DrawDebugSphere(GetWorld(), OwnerMesh->GetComponentLocation(), AirCheckRadius, 32, FColor::Yellow, false, .1f);

	return Hits.Num() == 0;
}

void UFireEngine::Move(const FInputActionValue& Value)
{
	IsMoving = true;

	FVector2D LookAxisVector = Value.Get<FVector2D>();
	LookAxisVector.Normalize();

	//FVector FinalDir = UKismetMathLibrary::TransformDirection(GetOwner()->GetTransform(), FVector(LookAxisVector.Y, LookAxisVector.X, 0));
	FVector FinalDir = OwnerMesh->GetForwardVector() * LookAxisVector.Y + OwnerMesh->GetRightVector() * LookAxisVector.X;
	FinalDir.Normalize();

	OwnerMesh->AddForce(FinalDir * FMath::Square(LateralMoveForce) * OwnerMesh->GetMass());
}

void UFireEngine::StopMove(const FInputActionValue& Value)
{
	IsMoving = false;
}

void UFireEngine::Throttle(const FInputActionValue& Value)
{
	IsThrottling = true;
	VerticalMovement(1);
}

void UFireEngine::EndThrottle(const FInputActionValue& Value)
{
	IsThrottling = false;
}

void UFireEngine::Reverse(const FInputActionValue& Value)
{
	if (IsThrottling) return;

	IsReversing = true;
	VerticalMovement(-1);
}

void UFireEngine::StopReverse(const FInputActionValue& Value)
{
	IsReversing = false;
}

void UFireEngine::VerticalMovement(float GravityMultiplier)
{
	FVector Throttle = OwnerMesh->GetUpVector();
	//Throttle.Normalize();
	Throttle *= GravityMultiplier;

	float ThrottleForce = MoveForce;

	if (ThrottleCurve)
	{
		ThrottleForce = ThrottleCurve->GetFloatValue(ThrottleCurveEvaluation) * MoveForce;

		ThrottleCurveEvaluation = FMath::Min(ThrottleCurveEvaluation + GetWorld()->GetDeltaSeconds(), 10); // random big number to not overflow float value
	}

	OwnerMesh->AddForce(Throttle * FMath::Square(ThrottleForce) * OwnerMesh->GetMass());

	//GEngine->AddOnScreenDebugMessage(-1, .1, FColor::Red, FString::Printf(TEXT("Throttle: %f - %f - %f"), Throttle.X, Throttle.Y, Throttle.Z));
}

void UFireEngine::StartLook(const FInputActionValue& Value)
{
}

void UFireEngine::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	IsLooking = true;

	RepositionTimer = 0;

	FVector FinalDir = UKismetMathLibrary::InverseTransformDirection(GetOwner()->GetTransform(), GetOwner()->GetActorUpVector()) * LookAxisVector.X;

	OwnerMesh->AddTorqueInDegrees(GetOwner()->GetActorUpVector() * LookAxisVector.X * FMath::Square(TorqueForce), NAME_None, true);

	FinalDir = UKismetMathLibrary::TransformDirection(GetOwner()->GetTransform(), GetOwner()->GetActorRightVector()) * LookAxisVector.Y;

	OwnerMesh->AddTorqueInDegrees(GetOwner()->GetActorRightVector() * LookAxisVector.Y * FMath::Square(TorqueForce), NAME_None, true);

	GEngine->AddOnScreenDebugMessage(-1, .1, FColor::Red, FString::Printf(TEXT("Looking: %f - %f - %f"), FinalDir.X, FinalDir.Y, FinalDir.Z));

}

void UFireEngine::StopLook(const FInputActionValue& Value)
{
	IsLooking = false;
}

void UFireEngine::AskReposition(FVector RepositionTorqueForce, bool ForceReposition)
{
	if(!ForceReposition)
	{
		//if (IsLooking || IsMoving) return;
		if (IsLooking) return;

		if (RepositionTimer < RepositionTimerThreshold)
		{
			RepositionTimer += GetWorld()->GetDeltaSeconds();
			return;
		}
	}

	IsRepositioning = true;

	OwnerMesh->AddTorqueInDegrees(RepositionTorqueForce, NAME_None, true);

	GEngine->AddOnScreenDebugMessage(-1, .1, FColor::Red, FString::Printf(TEXT("Repositioning")));
}

void UFireEngine::StopReposition()
{
	IsRepositioning = false;
}

void UFireEngine::NotifyAtmoForce(bool Active)
{
	if (!this || !OwnerMesh) return;

	if (!Active)
	{
		OnAtmoForce.Broadcast(FVector::ZeroVector, 0);
		return;
	}

	FVector Dir = GetOwner()->GetVelocity();
	float Magnitude = Dir.Length();
	Dir.Normalize();

	OnAtmoForce.Broadcast(Dir, Magnitude);
}

void UFireEngine::AdjustDirection()
{
	if (!(IsLooking || IsRepositioning))
	{
		FVector AngularVelocity = OwnerMesh->GetPhysicsAngularVelocityInDegrees();
		AngularVelocity = UKismetMathLibrary::VLerp(AngularVelocity, FVector::ZeroVector, GetWorld()->GetDeltaSeconds() * AngularVelocityDeterrent);

		OwnerMesh->SetPhysicsAngularVelocityInDegrees(AngularVelocity);

		//GEngine->AddOnScreenDebugMessage(-1, .1, FColor::White, FString::Printf(TEXT("Angular Velocity: %f - %f - %f"), AngularVelocity.X, AngularVelocity.Y, AngularVelocity.Z));
	}
}

