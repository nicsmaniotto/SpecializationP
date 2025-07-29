// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"
#include "Interaction.h"
#include "TransitionWidget.h"

void AMyHUD::BeginPlay()
{
	Super::BeginPlay();

	// Creating interaction widget
	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteraction>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(10);
	}

	// Creating transition widget
	if (TransitionWidgetClass)
	{
		TransitionWidget = CreateWidget<UTransitionWidget>(GetWorld(), TransitionWidgetClass);
		TransitionWidget->AddToViewport(100);

		TransitionWidget->OnMidtimeTransition.AddUniqueDynamic(this, &AMyHUD::OnMidtimeTransition);
		TransitionWidget->OnCompletedTransition.AddUniqueDynamic(this, &AMyHUD::OnCompletedTransition);
	}
	/*FTimerDelegate T;
	T.BindLambda([&, this]()->void {
		});

	GetWorld()->GetTimerManager().SetTimerForNextTick(T);*/
}

void AMyHUD::OnMidtimeTransition()
{
	OnTransition.ExecuteIfBound();
}

void AMyHUD::OnCompletedTransition()
{
	AskToggleWidget(false, EWidgetType::TRANSITION);

	OnTransitionEnd.ExecuteIfBound();
}

bool AMyHUD::AskToggleWidget(bool Active, EWidgetType Type, TSubclassOf<class UUserWidget> CustomWidget)
{
	switch (Type)
	{
	case EWidgetType::BASIC:
		break;
	case EWidgetType::SHIP:
		break;
	case EWidgetType::INTERACT:
	{
		if (!InteractionWidget) return false;

		int* Value;
		if (!StackableWidgets.Contains(InteractionWidget))
		{
			Value = &StackableWidgets.Add(InteractionWidget, 0);
		}
		else
		{
			Value = &StackableWidgets[InteractionWidget];
		}

		if (Active)
		{
			if (*Value == 0)
			{
				InteractionWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			}

			*Value += 1;
		}
		else
		{
			*Value = FMath::Max(*Value - 1, 0);

			if (*Value == 0)
			{
				InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
	break;
	case EWidgetType::TRANSITION:
	{
		if (Active)
		{
			if (TransitionWidget->GetVisibility() == ESlateVisibility::HitTestInvisible)
			{
				return false;
			}

			TransitionWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			TransitionWidget->MakeTransition();
		}
		else
		{
			TransitionWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	break;
	case EWidgetType::CUSTOM:
		break;
	default:
		break;
	}

	return true;
}

bool AMyHUD::ExecuteTransition(FOnTransition _OnTransition, FOnTransitionEnd _OnTransitionEnd)
{
	if (!TransitionWidget || !AskToggleWidget(true, EWidgetType::TRANSITION)) return false;

	OnTransition = _OnTransition;
	OnTransitionEnd = _OnTransitionEnd;

	return true;
}
