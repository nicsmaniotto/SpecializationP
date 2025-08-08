// Fill out your copyright notice in the Description page of Project Settings.


#include "Marker.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Camera/PlayerCameraManager.h"
#include "Components/WidgetComponent.h"

// Sets default values for this component's properties
UMarker::UMarker()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UMarker::BeginPlay()
{
	Super::BeginPlay();

	if (!IsMarking)
	{
		MarkedObject = CheckMarker();
	}

}

UWidgetComponent* UMarker::CheckMarker()
{
	FHitResult Hit;

	AActor* Camera = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetViewTarget();

	UKismetSystemLibrary::LineTraceSingle(GetWorld(), Camera->GetActorLocation(),
		Camera->GetActorLocation() + Camera->GetActorForwardVector() * CheckDistance, CheckType,
		false, { GetOwner() }, EDrawDebugTrace::ForDuration, Hit, true);

	float DotValue = -1;

	if (!Hit.bBlockingHit) return nullptr;

	UWidgetComponent* WC = Hit.GetActor()->GetComponentByClass<UWidgetComponent>();
	if (WC)
	{
		// activate
	}

	return WC;
}

// Called every frame
void UMarker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMarker::ToggleActivity(bool Active)
{
	PrimaryComponentTick.bCanEverTick = Active;
}

void UMarker::ToggleMarkObject()
{
	if (IsMarking)
	{
		// deactivate marked object
	}
	else
	{
		// activate marked object
	}

	IsMarking = !IsMarking;
}

FTransform UMarker::GetSelfTransform() const
{
	return GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetViewTarget()->GetTransform();
}

