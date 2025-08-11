// Fill out your copyright notice in the Description page of Project Settings.


#include "MarkerWidget.h"
#include "UMG.h"
#include "IndicatorWidget.h"

void UMarkerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	IndicatorMap.Add(EIndicatorType::LEFT, LeftIndicator);
	IndicatorMap.Add(EIndicatorType::TOP, TopIndicator);
	IndicatorMap.Add(EIndicatorType::RIGHT, RightIndicator);
	IndicatorMap.Add(EIndicatorType::BOTTOM, BottomIndicator);

	for (auto& Elem : IndicatorMap)
	{
		Elem.Value->ToggleIndicator(false);
		Elem.Value->SetIndicatorLength(0);
	}
}

void UMarkerWidget::ChangeBehavior_Implementation(ELockType LockType)
{
	for (auto& Elem : IndicatorMap)
	{
		Elem.Value->ToggleIndicator(LockType == ELockType::TRAJECTORY);
	}

	switch (LockType)
	{
	case ELockType::NONE:
		MarkerImg->SetBrush(NormalBrush);
		break;
	case ELockType::LOCKED:
		MarkerImg->SetBrush(LockedBrush);
		break;
	default:
		break;
	}
}

void UMarkerWidget::ShowTrajectoryForce(FVector Force)
{
	Force /= LengthDivider;

	IndicatorMap[EIndicatorType::LEFT]->SetIndicatorLength(FMath::Min(Force.Y, 0));
	IndicatorMap[EIndicatorType::TOP]->SetIndicatorLength(FMath::Max(Force.Z, 0));
	IndicatorMap[EIndicatorType::RIGHT]->SetIndicatorLength(FMath::Max(Force.Y, 0));
	IndicatorMap[EIndicatorType::BOTTOM]->SetIndicatorLength(FMath::Min(Force.Z, 0));
}


