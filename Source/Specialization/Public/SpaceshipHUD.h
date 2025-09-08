// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseMenu.h"
#include "SpaceshipHUD.generated.h"

 /**
 * @See BaseMenu
 * Spaceship HUD
 */
UCLASS()
class SPECIALIZATION_API USpaceshipHUD : public UBaseMenu
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	class USpaceshipMovUI* MovUI;
	
	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	class UTextBlock* ToggleJetpackTxt;
	
	/*Text for jetpack equip*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	FText EquipTxt;
	
	/*Text for jetpack unequip*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	FText UnequipTxt;

	UFUNCTION()
	void OnJetpackEquip(bool OnJetpack);

	virtual void SetupEvents_Implementation(AActor* LinkedActor) override;
	
};
