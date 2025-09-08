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
	/*class of the player HUD*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	TSubclassOf<UBaseMenu> BaseHUDWidgetClass;
	
	/*class of the spaceship HUD*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD")
	TSubclassOf<UBaseMenu> ShipHUDWidgetClass;
	
	/*class of the interaction widget: shared by all interactables*/
	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	UInteraction* InteractionWidget;

	/*class of the transition widget*/
	UPROPERTY(EditDefaultsOnly, Category = "Transition")
	TSubclassOf<UTransitionWidget> TransitionWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UBaseMenu* BaseHUDWidget;
	
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	UBaseMenu* ShipHUDWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TSubclassOf<UInteraction> InteractionWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Transition")
	UTransitionWidget* TransitionWidget;

	TMap<UUserWidget*, int> StackableWidgets;

	UFUNCTION()
	void OnMidtimeTransition();
	
	UFUNCTION()
	void OnCompletedTransition();

	FOnTransition OnTransition;
	FOnTransitionEnd OnTransitionEnd;

public:
	/* Manages the visualization of the desired widget according to the type specified. Use ExecuteTransition if you want to use the transition widget with custom events!*/
	UFUNCTION(BlueprintCallable)
	bool AskToggleWidget(bool Active, EWidgetType Type, TSubclassOf<class UUserWidget> CustomWidget = nullptr);
	
	/*
	* Manages the transition widget.
	* _OnTransition: mid-transition delegate,
	* _OnTransitionEnd: end-transition delegate,
	* returns if transition was executed successfully
	*/
	bool ExecuteTransition(FOnTransition _OnTransition, FOnTransitionEnd _OnTransitionEnd);

// Player
protected:
	UFUNCTION()
	void OnSpaceshipInteraction(bool OnSpaceship);
	
};
