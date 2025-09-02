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
 * 
 */
UCLASS()
class SPECIALIZATION_API UJetpackMenu : public UBaseMenu
{
	GENERATED_BODY()

protected:
	void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	UCanvasPanel* JetpackContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AppearAnim;
	
	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	UWidget* PropulsionSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* PropulsionAppearAnim;

	UFUNCTION()
	void OnJetpackEquip(bool IsEquipped);

	UFUNCTION()
	void OnGravityUpdate(FVector OldGravity, FVector NewGravity);

	bool IsJetpackEquipped;

public:
	void SetupEvents_Implementation(AActor* LinkedActor) override;
	
};
