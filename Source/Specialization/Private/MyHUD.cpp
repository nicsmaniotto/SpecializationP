// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"
#include "Interaction.h"
#include "TransitionWidget.h"
#include "BaseMenu.h"
#include "Specialization/SpecializationCharacter.h"
#include "Spaceship.h"
#include <Kismet/GameplayStatics.h>
#include <Setuppable.h>

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

	ASpecializationCharacter* Player = Cast<ASpecializationCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	// Creating HUD widgets
	if (BaseHUDWidgetClass)
	{
		BaseHUDWidget = CreateWidget<UBaseMenu>(GetWorld(), BaseHUDWidgetClass);
		BaseHUDWidget->AddToViewport(1);

		if (BaseHUDWidget->GetClass()->ImplementsInterface(USetuppable::StaticClass()))
		{
			ISetuppable::Execute_SetupEvents(BaseHUDWidget, Player);
		}
	}

	AActor* SS = UGameplayStatics::GetActorOfClass(GetWorld(), ASpaceship::StaticClass());

	if (ShipHUDWidgetClass)
	{
		ShipHUDWidget = CreateWidget<UBaseMenu>(GetWorld(), ShipHUDWidgetClass);
		ShipHUDWidget->AddToViewport(1);

		if (ShipHUDWidget->GetClass()->ImplementsInterface(USetuppable::StaticClass()))
		{
			ISetuppable::Execute_SetupEvents(ShipHUDWidget, SS);
		}
	}

	AskToggleWidget(true, EWidgetType::BASIC);

	if (Player)
	{
		Player->OnSpaceshipInteraction.AddUniqueDynamic(this, &AMyHUD::OnSpaceshipInteraction);
	}
}

void AMyHUD::OnSpaceshipInteraction(bool OnSpaceship)
{
	// switch betweens HUDs
	if (OnSpaceship)
	{
		AskToggleWidget(true, EWidgetType::SHIP);
	}
	else
	{
		AskToggleWidget(true, EWidgetType::BASIC);
	}
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
	{
		if (BaseHUDWidget)
		{
			BaseHUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}

		if (ShipHUDWidget)
		{
			ShipHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	break;
	case EWidgetType::SHIP:
	{
		if (BaseHUDWidget)
		{
			BaseHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (ShipHUDWidget)
		{
			ShipHUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
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
