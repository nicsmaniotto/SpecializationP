// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TransitionWidget.generated.h"

class UWidgetAnimation;

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMidtimeTransition);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompletedTransition);

/**
 * 
 */
UCLASS()
class SPECIALIZATION_API UTransitionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	void NativeConstruct() override;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeIn;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOut;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fade")
	float InBetweenTime = .5f;

	UFUNCTION()
	void MakeFadeOut();
	
	UFUNCTION()
	void FadeOutEvent();
	
	UFUNCTION()
	void OnFadeOutCompleted();

public:
	UPROPERTY(BlueprintAssignable)
	FOnMidtimeTransition OnMidtimeTransition;
	
	UPROPERTY(BlueprintAssignable)
	FOnCompletedTransition OnCompletedTransition;

	UFUNCTION(BlueprintCallable)
	void MakeTransition();
	
	
};
