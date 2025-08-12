// Fill out your copyright notice in the Description page of Project Settings.


#include "MarkingComponent.h"
#include "Marker.h"
#include "MarkerWidget.h"

UMarkingComponent::UMarkingComponent()
{
	SetVisibility(false);

	//PrimaryComponentTick.bCanEverTick = false;
}

void UMarkingComponent::BeginPlay()
{
	Super::BeginPlay();

	Setup();
}

void UMarkingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MarkerObj)
	{
		ApproachForces = CalcApproachForces();

		if (CurrentLockType == ELockType::TRAJECTORY)
		{
			MarkerWidget->ShowTrajectoryForce(ApproachForces);
		}
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
	//PrimaryComponentTick.bCanEverTick = !!Marker;

	CurrentLockType = !!Marker ? ELockType::LOCKED : ELockType::NONE;

	MarkerWidget->ChangeBehavior(CurrentLockType);

	/*if (Active)
	{
	}
	else
	{

	}*/

	MarkerObj = Marker;

	if (!MarkerObj) return;

	// ... on marker change
}

bool UMarkingComponent::ToggleTrajectory()
{
	if (CurrentLockType == ELockType::NONE) return false;

	CurrentLockType = CurrentLockType == ELockType::LOCKED ? ELockType::TRAJECTORY : ELockType::LOCKED;

	MarkerWidget->ChangeBehavior(CurrentLockType);

	return CurrentLockType == ELockType::TRAJECTORY;
}
