// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseMenu.h"
#include "SpaceshipHUD.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALIZATION_API USpaceshipHUD : public UBaseMenu
{
	GENERATED_BODY()


protected:
	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	class USpaceshipMovUI* MovUI;

	virtual void SetupEvents_Implementation(AActor* LinkedActor) override;
	
};
