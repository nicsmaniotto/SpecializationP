// Fill out your copyright notice in the Description page of Project Settings.


#include "MarkingComponent.h"
#include "Marker.h"
#include "MarkerWidget.h"

UMarkingComponent::UMarkingComponent()
{
	SetVisibility(false);
}

void UMarkingComponent::BeginPlay()
{
	Super::BeginPlay();

	Setup();
}

void UMarkingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MarkerObj && CurrentLockType == ELockType::LOCKED)
	{
		ApproachForces = CalcApproachForces();

		MarkerWidget->ShowTrajectoryForce(ApproachForces);
	}
}

void UMarkingComponent::Setup()
{
	// retrieve marker widget
	MarkerWidget = Cast<UMarkerWidget>(GetWidget());

	// assure owner is markable
	AActor* SC = GetOwner();

	if (!MarkerWidget || !SC || !SC->GetClass()->ImplementsInterface(UMarkable::StaticClass()))
	{
		DestroyComponent();
	}
	else
	{
		MarkableObj = SC;
	}
}

FVector UMarkingComponent::CalcApproachForces()
{
	USceneComponent* MO = IMarkable::Execute_GetMarkedObject(MarkableObj->_getUObject());

	FVector Dir = MO->GetComponentLocation() - MarkerObj->GetSelfTransform().GetLocation();
	Dir.Normalize();

	// Add component velocity to compensate the real time movement
	FVector FinalLoc = MO->GetComponentLocation() - Dir * ApproachDist + MO->GetComponentVelocity();
	DrawDebugSphere(GetWorld(), FinalLoc, 500, 12, FColor::Red, false, .1f);

	Dir = FinalLoc - MarkerObj->GetSelfTransform().GetLocation();

	return Dir;
}

void UMarkingComponent::ToggleVisualLock(bool Active)
{
	if(!Active) ToggleLock(nullptr);

	SetVisibility(Active);
}

void UMarkingComponent::ToggleLock(UMarker* Marker)
{
	CurrentLockType = !!Marker ? ELockType::LOCKED : ELockType::NONE;

	MarkerWidget->ChangeBehavior(CurrentLockType);

	MarkerObj = Marker;
}
