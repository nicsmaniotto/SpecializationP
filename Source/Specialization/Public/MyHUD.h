// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include <Enums.h>
#include "MyHUD.generated.h"

class UInteraction;
class UTransitionWidget;
class UBaseMenu;

DECLARE_DELEGATE(FOnTransition);

DECLARE_DELEGATE(FOnTransitionEnd);

/**
 * 
 */
UCLASS()
class SPECIALIZATION_API AMyHUD : public AHUD
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	TSubclassOf<UBaseMenu> BaseHUDWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	TSubclassOf<UBaseMenu> ShipHUDWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UBaseMenu* BaseHUDWidget;
	
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UBaseMenu* ShipHUDWidget;
	
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	UInteraction* InteractionWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TSubclassOf<UInteraction> InteractionWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, Category = "Transition")
	UTransitionWidget* TransitionWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Transition")
	TSubclassOf<UTransitionWidget> TransitionWidgetClass;

	TMap<UUserWidget*, int> StackableWidgets;

	UFUNCTION()
	void OnMidtimeTransition();
	
	UFUNCTION()
	void OnCompletedTransition();

	FOnTransition OnTransition;
	FOnTransitionEnd OnTransitionEnd;

public:
	UFUNCTION(BlueprintCallable)
	bool AskToggleWidget(bool Active, EWidgetType Type, TSubclassOf<class UUserWidget> CustomWidget = nullptr);
	
	bool ExecuteTransition(FOnTransition _OnTransition, FOnTransitionEnd _OnTransitionEnd);

// Player
protected:
	UFUNCTION()
	void OnSpaceshipInteraction(bool OnSpaceship);
	
};
