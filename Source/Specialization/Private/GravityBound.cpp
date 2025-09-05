// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityBound.h"
#include <Kismet/KismetMathLibrary.h>
#include <FireEngine.h>
#include <Kismet/KismetSystemLibrary.h>
#include <Possessable.h>
#include <Specialization/SpecializationGameMode.h>

UGravityBound::UGravityBound()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetTickGroup(ETickingGroup::TG_PostPhysics);
}

void UGravityBound::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddUniqueDynamic(this, &UGravityBound::OnBeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UGravityBound::OnEndOverlap);
}

void UGravityBound::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	EnlistComponent(OtherComp);
}

void UGravityBound::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UnenlistComponent(OtherComp);
}

void UGravityBound::EnlistComponent(UPrimitiveComponent* OtherComp)
{
	if (Overlaps.Contains(OtherComp)) return;
	//if (!OtherComp->IsSimulatingPhysics()) return;

	Overlaps.Add(OtherComp);

	UFireEngine* FireEngine = OtherComp->GetOwner()->GetComponentByClass<UFireEngine>();

	if (FireEngine)
	{
		LastGForces.Add(FireEngine, FVector());
	}
}

void UGravityBound::UnenlistComponent(UPrimitiveComponent* OtherComp)
{
	if (!Overlaps.Contains(OtherComp)) return;
	//if (!Forced && !OtherComp->IsSimulatingPhysics()) return;

	Overlaps.Remove(OtherComp);

	UFireEngine* FireEngine = OtherComp->GetOwner()->GetComponentByClass<UFireEngine>();

	if (FireEngine)
	{
		FireEngine->UpdateGravityForce(LastGForces[FireEngine], FVector::ZeroVector);

		FireEngine->NotifyAtmoForce(false);

		LastGForces.Remove(FireEngine);
	}
}

void UGravityBound::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GravityCurve) return;

	for (UPrimitiveComponent* P : Overlaps)
	{
		if (!P->IsSimulatingPhysics()) continue;

		UFireEngine* FireEngine = P->GetOwner()->GetComponentByClass<UFireEngine>();

		// Execute Gravity
		FVector Dir = ExecuteGravity(P, FireEngine);

		if(FireEngine) AskAlignement(IRepositionable::Execute_GetRepositionableComponent(FireEngine), FireEngine, Dir);

		AtmosphereVelChange(P, FireEngine, Dir);
	}
}

FVector UGravityBound::ExecuteGravity(UPrimitiveComponent* PrimitiveComponent, UFireEngine* FireEngine)
{
	FVector COG = UKismetMathLibrary::TransformLocation(GetComponentTransform(), CenterOfGravity);

	FVector Dir = COG - PrimitiveComponent->GetComponentLocation();
	float Dist = Dir.Length();
	Dir.Normalize();

	float GForce = GravityCurve->GetFloatValue(Dist);

	FVector NewGForce = Dir * GForce * ASpecializationGameMode::GRAVITYMULTIPLIER;
	PrimitiveComponent->AddForce(NewGForce, NAME_None, true);

	DrawDebugLine(GetWorld(), PrimitiveComponent->GetComponentLocation(), PrimitiveComponent->GetComponentLocation() + Dir * Dist, FColor::Red, false, .1f);

	// Update engine gravity force
	if (FireEngine)
	{
		FVector OldGForce = LastGForces[FireEngine];

		LastGForces[FireEngine] = NewGForce;

		FireEngine->UpdateGravityForce(OldGForce, LastGForces[FireEngine]);
	}

	return Dir;
}

void UGravityBound::AskAlignement(USceneComponent* PrimitiveComponent, UFireEngine* FireEngine, FVector Dir)
{
	if (!FireEngine || AllowedRepositionTypes.Num() == 0) return;

	if (bReverseAlignmentDir) Dir *= -1;

	if (bFaceDir) Dir = -FVector::VectorPlaneProject(PrimitiveComponent->GetUpVector(), -Dir);

	FVector RefAxisForward = FVector::VectorPlaneProject(PrimitiveComponent->GetForwardVector(), -Dir);

	FVector RefAxisRight = -FVector::CrossProduct(RefAxisForward, -Dir);

	FVector TorqueVector = FVector::ZeroVector;

	/*DrawDebugLine(GetWorld(), PrimitiveComponent->GetComponentLocation(), PrimitiveComponent->GetComponentLocation() + PrimitiveComponent->GetRightVector() * 200, FColor::Green, false, .1f);
	DrawDebugLine(GetWorld(), PrimitiveComponent->GetComponentLocation(), PrimitiveComponent->GetComponentLocation() + RefAxisForward * 200, FColor::Red, false, .1f);
	DrawDebugLine(GetWorld(), PrimitiveComponent->GetComponentLocation(), PrimitiveComponent->GetComponentLocation() + -Dir * 200, FColor::Blue, false, .1f);*/

	if (1 - FVector::DotProduct(-Dir, PrimitiveComponent->GetUpVector()) > .02f)
	{
		if (AllowedRepositionTypes.Contains(ERepositionType::RIGHT) && FVector::CrossProduct(RefAxisForward, PrimitiveComponent->GetForwardVector()).SquaredLength() > FMath::Square(.02f))
		{
			if (FVector::DotProduct(PrimitiveComponent->GetForwardVector(), -Dir) > 0)
			{
				TorqueVector = PrimitiveComponent->GetRightVector();
			}
			else
			{
				TorqueVector = -PrimitiveComponent->GetRightVector();
			}

			IRepositionable::Execute_AskReposition(FireEngine, ERepositionType::RIGHT, TorqueVector * RedirectionForce, ForceReposition);
		}

		if (AllowedRepositionTypes.Contains(ERepositionType::FORWARD) && FVector::CrossProduct(RefAxisRight, PrimitiveComponent->GetRightVector()).SquaredLength() > FMath::Square(.02f))
		{
			if (FVector::DotProduct(PrimitiveComponent->GetRightVector(), -Dir) > 0)
			{
				TorqueVector = -PrimitiveComponent->GetForwardVector();
			}
			else
			{
				TorqueVector = PrimitiveComponent->GetForwardVector();
			}

			IRepositionable::Execute_AskReposition(FireEngine, ERepositionType::FORWARD, TorqueVector * RedirectionForce, ForceReposition);
		}

		//GEngine->AddOnScreenDebugMessage(-1, .1, FColor::Red, FString::Printf(TEXT("Dist: %f - Force: %f - Dot: %f "), Dist, GravityForce, FVector::CrossProduct(RefAxisRight, P->GetRightVector()).Length(), .05f));
	}
	else
	{
		FireEngine->StopReposition();
	}
}

void UGravityBound::AtmosphereVelChange(UPrimitiveComponent* PrimitiveComponent, UFireEngine* FireEngine, FVector Dir)
{
	if (!HasAtmoVelChange) return;

	FVector Start = GetComponentLocation() - GetUpVector() * (AtmoCheckHeight - AtmoCheckRadius);
	FVector End = GetComponentLocation() + GetUpVector() * (AtmoCheckHeight - AtmoCheckRadius);

	TArray<FHitResult> Hits;

	if (ShowAtmoInner)
	{
		UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End, AtmoCheckRadius, { UEngineTypes::ConvertToObjectType(ECC_PhysicsBody) },
			false, { GetOwner() }, EDrawDebugTrace::ForOneFrame, Hits, true, FLinearColor::Yellow, FLinearColor::Yellow);
	}

	bool Found = false;
	for (FHitResult& Hit : Hits)
	{
		if (Hit.GetComponent() == PrimitiveComponent)
		{
			Found = true;
			break;
		}
	}

	if (Found)
	{
		FireEngine->NotifyAtmoForce(false);
		return;
	}

	FireEngine->NotifyAtmoForce(true);

	FVector VelocityDir = PrimitiveComponent->GetPhysicsLinearVelocity();

	if (VelocityDir.SquaredLength() < FMath::Square(MinAtmoVelocity)) return;

	VelocityDir.Normalize();

	float DotP = FVector::DotProduct(VelocityDir, -Dir);

	if (FMath::IsNearlyZero(DotP, AtmoDotValue))
	{
		VelocityDir = FVector::VectorPlaneProject(VelocityDir, -Dir);

		PrimitiveComponent->AddForce(VelocityDir * PrimitiveComponent->GetMass() * VelChangeValue);
	}
}