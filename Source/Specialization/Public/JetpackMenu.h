// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseMenu.h"
#include "JetpackMenu.generated.h"

class UBaseHUD;
class UCanvasPanel;
class URadialSlider;

/**
* @see class BaseMenu
 * Player HUD
 */
UCLASS()
class SPECIALIZATION_API UJetpackMenu : public UBaseMenu
{
	GENERATED_BODY()

protected:
	void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AppearAnim;
	
	/*Slider of the jetpack propulsion*/
	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	UWidget* PropulsionSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* PropulsionAppearAnim;

	UFUNCTION()
	void OnJetpackEquip(bool IsEquipped);

	UFUNCTION()
	void OnAtmosphereUpdate(class APlanet* Planet);

	bool IsJetpackEquipped;

public:
	void SetupEvents_Implementation(AActor* LinkedActor) override;
	
};
