// Fill out your copyright notice in the Description page of Project Settings.


#include "FireEngine.h"
#include <Kismet/KismetMathLibrary.h>
#include <InputActionValue.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Marker.h>
#include <MarkingComponent.h>
#include <Planet.h>

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

	TArray<UActorComponent*> Components;

	Components = GetOwner()->K2_GetComponentsByClass(UPrimitiveComponent::StaticClass());

	for (UActorComponent* AC : Components)
	{
		UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(AC);

		if (Primitive && Primitive->IsSimulatingPhysics())
		{
			OwnerPhysicsComponent = Primitive;
			break;
		}
	}

	if (!OwnerPhysicsComponent) return;

	NormalLinearDamping = OwnerPhysicsComponent->GetLinearDamping();

	OwnerPhysicsComponent->OnComponentHit.AddUniqueDynamic(this, &UFireEngine::LandHelper);

	OwnerPhysicsComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &UFireEngine::OnBeginOverlap);
	OwnerPhysicsComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &UFireEngine::OnEndOverlap);
}


// Called every frame
void UFireEngine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//AdjustDirection();

	OnAir = AirChecker();

	OwnerPhysicsComponent->SetNotifyRigidBodyCollision(OnAir);

	if (!(OnAir || IsThrottling))
	{
		ThrottleCurveEvaluation = 0;
	}

	if (IsAutomatic) AutomaticPilotMovement();
}

FRotator UFireEngine::GetDirectionRotation() const
{
	return OwnerPhysicsComponent->GetComponentRotation();
}

bool UFireEngine::AirChecker()
{
	FVector Loc = UKismetMathLibrary::TransformLocation(GetOwner()->GetTransform(), FeetPosition);
	FVector Dir = GravityForce.GetSafeNormal();

	if (Dir.SquaredLength() == 0)
	{
		Dir == -OwnerPhysicsComponent->GetUpVector();
	}

	//UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);

	UKismetSystemLibrary::LineTraceSingle(GetWorld(), Loc, Loc + Dir * AirCheckLength, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldStatic), false,
		{ GetOwner() }, EDrawDebugTrace::ForOneFrame, SurfaceHit, true, FColor::Blue);

	/*UKismetSystemLibrary::SphereOverlapComponents(GetWorld(), Loc, AirCheckRadius,
		{ EObjectTypeQuery::ObjectTypeQuery1 }, UStaticMeshComponent::StaticClass(), { GetOwner() }, Hits);*/

	//DrawDebugSphere(GetWorld(), Loc, AirCheckRadius, 32, FColor::Emerald, false, .01f);

	return !SurfaceHit.bBlockingHit;
}

void UFireEngine::LandHelper(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FVector LinearVelocity = OwnerPhysicsComponent->GetPhysicsLinearVelocity();
	if (LinearVelocity.Length() > LandingVelocityThreshold) return;

	//OwnerPhysicsComponent->SetPhysicsLinearVelocity(LinearVelocity / LandingLinearVelocityDivider);
	//OwnerPhysicsComponent->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
	//OwnerPhysicsComponent->SetPhysicsAngularVelocityInDegrees(OwnerPhysicsComponent->GetPhysicsAngularVelocityInDegrees() / LandingAngularVelocityDivider);
	OwnerPhysicsComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
}

void UFireEngine::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlanet* Planet = Cast<APlanet>(OtherActor);
	if (Planet)
	{
		LandingPlanet = Planet;
	}
}

void UFireEngine::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlanet* Planet = Cast<APlanet>(OtherActor);
	if (Planet)
	{
		LandingPlanet = nullptr;
	}
}

void UFireEngine::Move(const FInputActionValue& Value)
{
	if (IsAutomatic) return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();
	//LookAxisVector.Normalize();

	if (HorizontalMovement(LookAxisVector))
	{
		IsMoving = true;
	}
}

void UFireEngine::StopMove(const FInputActionValue& Value)
{
	IsMoving = false;

	if (OnLateralMovement.IsBound()) OnLateralMovement.Broadcast(FVector::ZeroVector, 0, OwnerPhysicsComponent->GetComponentTransform());
}

void UFireEngine::Throttle(const FInputActionValue& Value)
{
	if (IsAutomatic) return;

	float AxisValue = Value.Get<float>();
	//GEngine->AddOnScreenDebugMessage(-1, .1, FColor::Red, FString::Printf(TEXT("Axis Val: %f"), AxisValue));

	if (VerticalMovement(AxisValue))
	{
		IsThrottling = true;
	}
}

void UFireEngine::EndThrottle(const FInputActionValue& Value)
{
	IsThrottling = false;

	if (!IsReversing && OnVerticalMovement.IsBound()) OnVerticalMovement.Broadcast(FVector::ZeroVector, 0, OwnerPhysicsComponent->GetComponentTransform());
}

void UFireEngine::Reverse(const FInputActionValue& Value)
{
	if (IsAutomatic) return;

	if (IsThrottling)
	{
		if (IsReversing) StopReverse(Value);

		return;
	}

	float AxisValue = Value.Get<float>();

	if (VerticalMovement(AxisValue))
	{
		IsReversing = true;
	}
}

void UFireEngine::StopReverse(const FInputActionValue& Value)
{
	IsReversing = false;

	if (!IsThrottling && OnVerticalMovement.IsBound()) OnVerticalMovement.Broadcast(FVector::ZeroVector, 0, OwnerPhysicsComponent->GetComponentTransform());
}

bool UFireEngine::HorizontalMovement(FVector2D LookAxisVector)
{
	FVector FinalDir = GetDirectionRotation().Quaternion().GetForwardVector() * LookAxisVector.Y + GetDirectionRotation().Quaternion().GetRightVector() * LookAxisVector.X;

	OwnerPhysicsComponent->AddForce(FinalDir * FMath::Square(LateralMoveForce) * OwnerPhysicsComponent->GetMass());

	FTransform t = OwnerPhysicsComponent->GetComponentTransform();
	t.SetRotation(GetDirectionRotation().Quaternion());

	if (OnLateralMovement.IsBound()) OnLateralMovement.Broadcast(FinalDir, LateralMoveForce, t);

	return true;
}

bool UFireEngine::VerticalMovement(float GravityMultiplier)
{
	FVector Throttle = GetDirectionRotation().Quaternion().GetUpVector();
	//Throttle.Normalize();
	Throttle *= GravityMultiplier;

	float ThrottleForce = MoveForce;

	if (ThrottleCurve)
	{
		ThrottleForce = ThrottleCurve->GetFloatValue(ThrottleCurveEvaluation) * MoveForce;

		ThrottleCurveEvaluation = FMath::Min(ThrottleCurveEvaluation + GetWorld()->GetDeltaSeconds(), 10); // random big number to not overflow float value
	}

	OwnerPhysicsComponent->AddForce(Throttle * FMath::Square(ThrottleForce) * OwnerPhysicsComponent->GetMass());

	FTransform t = OwnerPhysicsComponent->GetComponentTransform();
	t.SetRotation(GetDirectionRotation().Quaternion());

	if (OnVerticalMovement.IsBound()) OnVerticalMovement.Broadcast(Throttle, ThrottleForce * MoveForce, t);

	return true;
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

	OwnerPhysicsComponent->AddTorqueInDegrees(GetOwner()->GetActorUpVector() * LookAxisVector.X * FMath::Square(TorqueForce), NAME_None, true);

	FinalDir = UKismetMathLibrary::TransformDirection(GetOwner()->GetTransform(), GetOwner()->GetActorRightVector()) * LookAxisVector.Y;

	OwnerPhysicsComponent->AddTorqueInDegrees(GetOwner()->GetActorRightVector() * LookAxisVector.Y * FMath::Square(TorqueForce), NAME_None, true);

}

void UFireEngine::StopLook(const FInputActionValue& Value)
{
	IsLooking = false;
}

void UFireEngine::AskReposition_Implementation(ERepositionType RepositionType, FVector RepositionTorqueForce, bool ForceReposition)
{
	if (!ForceReposition)
	{
		//if (IsLooking || IsMoving) return;
		if (IsLooking || !RepositionTypes.Contains(RepositionType)) return;

		if (RepositionTimer < RepositionTimerThreshold)
		{
			RepositionTimer += GetWorld()->GetDeltaSeconds();
			return;
		}
	}

	IsRepositioning = true;

	UPrimitiveComponent* PC = Cast<UPrimitiveComponent>(IRepositionable::Execute_GetRepositionableComponent(this));

	if (!PC) return;

	PC->AddTorqueInDegrees(RepositionTorqueForce, NAME_None, true);

	GEngine->AddOnScreenDebugMessage(-1, .1, FColor::Red, FString::Printf(TEXT("Repositioning")));

}

void UFireEngine::StopReposition()
{
	IsRepositioning = false;
}

void UFireEngine::UpdateGravityForce(FVector OldGForce, FVector NewGForce)
{
	FVector OldForce = GravityForce;

	GravityForce -= OldGForce;
	GravityForce += NewGForce;

	if (IsAutomatic && GravityForce.SquaredLength() > 0)
	{
		ToggleAutomaticPilot(false);
	}

	OwnerPhysicsComponent->SetLinearDamping(GravityForce.SquaredLength() == 0 ? SpaceLinearDamping : NormalLinearDamping);

	OnGravityUpdate.Broadcast(OldForce, GravityForce);
}

void UFireEngine::ToggleAutomaticPilot(bool Active)
{
	if (IsAutomatic == Active) return;
	if (Active && IsInAtmosphere()) return;

	IsAutomatic = Active;

	if(OnAutomaticPilot.IsBound()) OnAutomaticPilot.Broadcast(IsAutomatic);

	if (!Active)
	{
		IsRetroFireActivated = false;

		OwnerPhysicsComponent->SetLinearDamping(SpaceLinearDamping);
	}
	else
	{
		OwnerPhysicsComponent->SetLinearDamping(NormalLinearDamping);
	}
}

void UFireEngine::AutomaticPilotMovement()
{
	UMarkingComponent* MC = Marker->GetMarkedObject();

	if (!Marker->GetIsMarking() || !MC)
	{
		ToggleAutomaticPilot(false);
		return;
	}

	FVector ApproachForces = MC->GetApproachForces();
	float FinalDist = ApproachForces.SquaredLength();

	if (ApproachForces.SquaredLength() < FMath::Square(AutomaticApproachAcceptance))
	{
		ToggleAutomaticPilot(false);
		return;
	}

	FVector PhysicsVelocity = OwnerPhysicsComponent->GetPhysicsLinearVelocity();

	FVector RelativeDir = UKismetMathLibrary::InverseTransformDirection(OwnerPhysicsComponent->GetComponentTransform(), ApproachForces);
	RelativeDir.Normalize();

	FVector2D XYForce = FVector2D(RelativeDir.Y, RelativeDir.X);
	
	float TempDistToStop = (PhysicsVelocity.SquaredLength() * OwnerPhysicsComponent->GetLinearDamping()) / (2.35f * (ApproachForces.GetSafeNormal() * MoveForce).SquaredLength());

	if (!IsRetroFireActivated && FVector::DotProduct(PhysicsVelocity, ApproachForces) > 0 && FMath::Square(TempDistToStop) >= ApproachForces.SquaredLength())
	{
		IsRetroFireActivated = true;
	}

	if (IsRetroFireActivated)
	{
		XYForce *= -1;
		RelativeDir.Z *= -1;
	}

	if (IsRetroFireActivated && FVector::DotProduct(PhysicsVelocity, ApproachForces) < 0)
	{
		ToggleAutomaticPilot(false);
		return;
	}

	bool MovFlag = true;

	if (XYForce.SquaredLength() > 0)
	{
		MovFlag = MovFlag && HorizontalMovement(XYForce.GetSafeNormal());
	}

	if (MovFlag && ApproachForces.Z > 0)
	{
		MovFlag = MovFlag && VerticalMovement(RelativeDir.Z);
	}

	if (!MovFlag)
	{
		ToggleAutomaticPilot(false);
	}
}

void UFireEngine::SetDependencyComponent(UMarker* MarkerComponent)
{
	Marker = MarkerComponent;
}

void UFireEngine::NotifyAtmoForce(bool Active)
{
	if (!this || !OwnerPhysicsComponent) return;

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
//
//void UFireEngine::AdjustDirection()
//{
//	if (!(IsLooking || IsRepositioning))
//	{
//		FVector AngularVelocity = OwnerPhysicsComponent->GetPhysicsAngularVelocityInDegrees();
//		AngularVelocity = UKismetMathLibrary::VLerp(AngularVelocity, FVector::ZeroVector, GetWorld()->GetDeltaSeconds() * AngularVelocityDeterrent);
//
//		//OwnerPhysicsComponent->SetPhysicsAngularVelocityInDegrees(AngularVelocity);
//
//		//GEngine->AddOnScreenDebugMessage(-1, .1, FColor::White, FString::Printf(TEXT("Angular Velocity: %f - %f - %f"), AngularVelocity.X, AngularVelocity.Y, AngularVelocity.Z));
//	}
//}

