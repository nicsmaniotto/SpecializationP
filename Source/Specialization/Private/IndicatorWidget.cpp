// Fill out your copyright notice in the Description page of Project Settings.


#include "IndicatorWidget.h"
#include "UMG.h"

void UIndicatorWidget::SetIndicatorLength(float Length)
{
	UCanvasPanelSlot* S = Cast<UCanvasPanelSlot>(Slot);
	FVector2D Size = S->GetSize();

	float NormalizedLength = FMath::Abs(Length);

	if (IsVertical)
	{
		Size.Y = FMath::Min(NormalizedLength, MaxSize);
	}
	else
	{
		Size.X = FMath::Min(NormalizedLength, MaxSize);
	}

	S->SetSize(Size);
}

void UIndicatorWidget::ToggleIndicator(bool Active)
{
	SetVisibility(Active ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}
