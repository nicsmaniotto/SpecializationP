// Fill out your copyright notice in the Description page of Project Settings.


#include "Marker.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/KismetMathLibrary.h>
#include "Camera/PlayerCameraManager.h"
#include "Components/WidgetComponent.h"
#include "MarkingComponent.h"

// Sets default values for this component's properties
UMarker::UMarker()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UMarker::BeginPlay()
{
	Super::BeginPlay();
}

void UMarker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APlayerController* Controller = CheckController();

	if (!Controller)
	{
		if (MarkedObject)
		{
			MarkedObject->ToggleVisualLock(false);
			MarkedObject = nullptr;
		}

		return;
	}

	if (!IsMarking)
	{
		UMarkingComponent* WC = CheckMarker(Controller);

		if (!WC)
		{
			if (MarkedObject)
			{
				MarkedObject->ToggleVisualLock(false);
				MarkedObject = WC;
			}

			return;
		}

		if (MarkedObject)
		{
			if (MarkedObject != WC)
			{
				MarkedObject->ToggleVisualLock(false);
			}
			else
			{
				return;
			}
		}

		MarkedObject = WC;

		MarkedObject->ToggleVisualLock(true);
	}
}

APlayerController* UMarker::CheckController()
{
	AController* Controller = Cast<APawn>(GetOwner())->GetController();

	if (!Controller || !Controller->IsLocalPlayerController())
	{
		return nullptr;
	}

	return Cast<APlayerController>(Controller);
}

UMarkingComponent* UMarker::CheckMarker(APlayerController* PlayerController)
{
	FHitResult Hit;

	FRotator Rot = PlayerController->PlayerCameraManager->GetCameraRotation();
	FVector Loc = PlayerController->PlayerCameraManager->GetCameraLocation();
	FVector Dir = UKismetMathLibrary::Quat_RotateVector(Rot.Quaternion(), FVector::ForwardVector);

	UKismetSystemLibrary::LineTraceSingle(GetWorld(), Loc,
		Loc + Dir * CheckDistance, CheckType,
		false, { GetOwner() }, EDrawDebugTrace::ForOneFrame, Hit, true);

	float DotValue = -1;

	if (!Hit.bBlockingHit) return nullptr;

	return Hit.GetActor()->GetComponentByClass<UMarkingComponent>();
}

void UMarker::ToggleMarkObject()
{
	if (IsMarking)
	{
		MarkedObject->ToggleLock(nullptr);
	}
	else
	{
		if (!MarkedObject) return;

		MarkedObject->ToggleLock(this);
	}

	IsMarking = !IsMarking;
}

bool UMarker::ToggleTrajectory()
{
	if (!IsMarking || !MarkedObject) return false;

	return MarkedObject->ToggleTrajectory();
}

FTransform UMarker::GetSelfTransform() const
{
	return GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetViewTarget()->GetTransform();
}

void UMarker::ToggleSelf(bool Active)
{
	SetComponentTickEnabled(Active);

	if (!Active && !!MarkedObject)
	{
		MarkedObject->ToggleVisualLock(false);
		MarkedObject->ToggleLock(nullptr);
		MarkedObject = nullptr;

		IsMarking = false;
	}
}

