// Fill out your copyright notice in the Description page of Project Settings.


#include "MarkingComponent.h"
#include "Marker.h"

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
	}
}

void UMarkingComponent::Setup()
{
	USceneComponent* SC = GetAttachParent();

	if (SC->GetClass()->ImplementsInterface(UMarkable::StaticClass()))
	{
		MarkableObj = SC;
	}
	else
	{
		DestroyComponent();
	}
}

FVector UMarkingComponent::CalcApproachForces()
{
	USceneComponent* MO = IMarkable::Execute_GetMarkedObject(MarkableObj->_getUObject());

	FVector Dir = MO->GetComponentLocation() - MarkerObj->GetSelfTransform().GetLocation();
	Dir.Normalize();

	FVector FinalLoc = MO->GetComponentLocation() - Dir * ApproachDist;
	Dir = FinalLoc - MarkerObj->GetSelfTransform().GetLocation();
	Dir += MO->GetComponentVelocity();

	return Dir;
}

void UMarkingComponent::ToggleVisualLock(bool Active)
{
	SetVisibility(Active);
}

void UMarkingComponent::ToggleLock(bool Active, UMarker* Marker)
{
	if (Active)
	{

	}
	else
	{

	}

	MarkerObj = Marker;

	if (!MarkerObj) return;

	// ... on marker change
}
