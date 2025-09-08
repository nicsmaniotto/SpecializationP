// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipMovUI.h"
#include "FireEngine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Specialization/SpecializationCharacter.h"

void USpaceshipMovUI::NativeConstruct()
{
	BarPositions.Add(EBarPosition::FRONT, FrontProgressBar);
	BarPositions.Add(EBarPosition::BACK, BackProgressBar);
	BarPositions.Add(EBarPosition::TOP, TopProgressBar);
	BarPositions.Add(EBarPosition::BOTTOM, BottomProgressBar);
	BarPositions.Add(EBarPosition::RIGHT, RightProgressBar);
	BarPositions.Add(EBarPosition::LEFT, LeftProgressBar);
}

void USpaceshipMovUI::SetupEvents_Implementation(AActor* LinkedActor)
{
	UFireEngine* FE = LinkedActor->GetComponentByClass<UFireEngine>();

	// links to movement
	if (FE)
	{
		FE->OnVerticalMovement.AddUniqueDynamic(this, &USpaceshipMovUI::OnVerticalMovement);
		FE->OnLateralMovement.AddUniqueDynamic(this, &USpaceshipMovUI::OnLateralMovement);
		FE->OnAutomaticPilot.AddUniqueDynamic(this, &USpaceshipMovUI::OnAutomaticPilot);
	}

	ASpecializationCharacter* Player = Cast<ASpecializationCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (!Player) return;

	Player->OnSpaceshipInteraction.AddUniqueDynamic(this, &USpaceshipMovUI::OnSpaceshipInteraction);
}

void USpaceshipMovUI::OnSpaceshipInteraction(bool OnSpaceship)
{
	if (!OnSpaceship)
	{
		ResetPercentages();
	}
}

void USpaceshipMovUI::OnAutomaticPilot(bool Active)
{
	// reset bars when automatic pilot ends
	if (!Active)
	{
		ResetPercentages();
	}
}

void USpaceshipMovUI::ResetPercentages()
{
	for (auto& Elem : BarPositions)
	{
		Elem.Value->SetPercent(0);
	}
}

void USpaceshipMovUI::OnVerticalMovement(FVector WorldDir, float Magnitude, FTransform CallingTransform)
{
	// find relative position of the movement
	FVector RelativeDir = UKismetMathLibrary::InverseTransformDirection(CallingTransform, WorldDir);

	EBarPosition Pos = EBarPosition::TOP;

	if (RelativeDir.Z < 0)
	{
		Pos = EBarPosition::BOTTOM;
	}

	FillBar(Pos, RelativeDir.Z);
}

void USpaceshipMovUI::OnLateralMovement(FVector WorldDir, float Magnitude, FTransform CallingTransform)
{
	// find relative position of the movement
	FVector RelativeDir = UKismetMathLibrary::InverseTransformDirection(CallingTransform, WorldDir);

	EBarPosition Pos = EBarPosition::FRONT;

	if (RelativeDir.X < 0)
	{
		Pos = EBarPosition::BACK;
	}

	FillBar(Pos, RelativeDir.X);

	Pos = EBarPosition::RIGHT;

	if (RelativeDir.Y < 0)
	{
		Pos = EBarPosition::LEFT;
	}

	FillBar(Pos, RelativeDir.Y);
}

UProgressBar* USpaceshipMovUI::GetOpposingBar(EBarPosition Position) const
{
	switch (Position)
	{
	case EBarPosition::FRONT:
		return BarPositions[EBarPosition::BACK];
	case EBarPosition::BACK:
		return BarPositions[EBarPosition::FRONT];
	case EBarPosition::TOP:
		return BarPositions[EBarPosition::BOTTOM];
	case EBarPosition::BOTTOM:
		return BarPositions[EBarPosition::TOP];
	case EBarPosition::RIGHT:
		return BarPositions[EBarPosition::LEFT];
	case EBarPosition::LEFT:
		return BarPositions[EBarPosition::RIGHT];
	default:
		return BarPositions[EBarPosition::FRONT];
	}
}

void USpaceshipMovUI::FillBar(EBarPosition Position, float Perc)
{
	// fill bar and reset opposing
	BarPositions[Position]->SetPercent(FMath::Abs(Perc));
	GetOpposingBar(Position)->SetPercent(0);
}
