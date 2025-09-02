// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipWidgetComponent.h"

USpaceshipWidgetComponent::USpaceshipWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpaceshipWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (!PlayerPawn) DestroyComponent();
}

void USpaceshipWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PlayerPawn) return;

	bool IsPlayerClose = CheckPlayerClose();

	if (!IsPlayerClose && bHiddenInGame)
	{
		SetHiddenInGame(false);
		return;
	}

	if (IsPlayerClose && !bHiddenInGame)
	{
		SetHiddenInGame(true);
	}
}

bool USpaceshipWidgetComponent::CheckPlayerClose() const
{
	return (PlayerPawn->GetActorLocation() - GetComponentLocation()).SquaredLength() <= FMath::Square(CloseDistance);
}
