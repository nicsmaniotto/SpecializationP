// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IndicatorWidget.generated.h"

/**
 * @see class MarkerWidget
 * Visual indicators of approach forces used by a fire engine automatic pilot
 */
UCLASS()
class SPECIALIZATION_API UIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Indicator")
	bool IsVertical;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Indicator")
	float MaxSize = 200;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Indicator")
	FSlateBrush VerticalBrush;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Indicator")
	FSlateBrush HorizontalBrush;

public:
	UFUNCTION(BlueprintCallable)
	void SetIndicatorLength(float Length);
	
	UFUNCTION(BlueprintCallable)
	void ToggleIndicator(bool Active);
};
