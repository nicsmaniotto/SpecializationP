// Fill out your copyright notice in the Description page of Project Settings.


#include "TransitionWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Animation/WidgetAnimationEvents.h"

void UTransitionWidget::NativeConstruct()
{
	FWidgetAnimationDynamicEvent FadeInCompletion;
	FadeInCompletion.BindDynamic(this, &UTransitionWidget::MakeFadeOut);

	BindToAnimationFinished(FadeIn, FadeInCompletion);

	FWidgetAnimationDynamicEvent FadeOutCompletion;
	FadeOutCompletion.BindDynamic(this, &UTransitionWidget::OnFadeOutCompleted);

	BindToAnimationFinished(FadeOut, FadeOutCompletion);
}

void UTransitionWidget::MakeFadeOut()
{
	if (OnMidtimeTransition.IsBound()) OnMidtimeTransition.Broadcast();

	FTimerHandle Handle;

	FTimerDelegate T;
	T.BindUObject(this, &UTransitionWidget::FadeOutEvent);

	GetWorld()->GetTimerManager().SetTimer(Handle, T, InBetweenTime, false);
}

void UTransitionWidget::FadeOutEvent()
{
	PlayAnimationForward(FadeOut);
}

void UTransitionWidget::OnFadeOutCompleted()
{
	if (OnCompletedTransition.IsBound()) OnCompletedTransition.Broadcast();
}

void UTransitionWidget::MakeTransition()
{
	PlayAnimationForward(FadeIn);
}
