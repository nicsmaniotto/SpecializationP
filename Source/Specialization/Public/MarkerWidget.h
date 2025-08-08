// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MarkerWidget.generated.h"

struct FSlateBrush;

/**
 * 
 */
UCLASS()
class SPECIALIZATION_API UMarkerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker")
	FSlateBrush NormalBrush;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker")
	FSlateBrush LockedBrush;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void ChangeBehavior(bool IsLocked);
	
};
