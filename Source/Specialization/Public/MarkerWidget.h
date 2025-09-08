// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enums.h"
#include "MarkerWidget.generated.h"

class UImage;
class UIndicatorWidget;
struct FSlateBrush;

/**
 * @See class MarkingComponent
 * @See class Marker
 * Widget that is applied to a marking component to visualize the actual marked object
 */
UCLASS()
class SPECIALIZATION_API UMarkerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	void NativeConstruct() override;

protected:
	/*Default brush of the marked object*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker")
	FSlateBrush NormalBrush;
	
	/*Locked brush of the marked object*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker")
	FSlateBrush LockedBrush;
	
	UPROPERTY(BlueprintReadWrite, Category = "Marker", meta = (BindWidget))
	UImage* MarkerImg;
	
	// Indicators
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Marker", meta = (BindWidget))
	UIndicatorWidget* LeftIndicator;
	
	UPROPERTY(BlueprintReadWrite, Category = "Marker", meta = (BindWidget))
	UIndicatorWidget* RightIndicator;

	UPROPERTY(BlueprintReadWrite, Category = "Marker", meta = (BindWidget))
	UIndicatorWidget* TopIndicator;

	UPROPERTY(BlueprintReadWrite, Category = "Marker", meta = (BindWidget))
	UIndicatorWidget* BottomIndicator;

	UPROPERTY(BlueprintReadWrite, Category = "Marker")
	TMap<EIndicatorType, UIndicatorWidget*> IndicatorMap;

	/*Divider of the length of the approach forces along horizontal/vertical*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Indicator")
	float LengthDivider = 500;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ChangeBehavior(ELockType LockType);
	
	void ChangeBehavior_Implementation(ELockType LockType);

	UFUNCTION(BlueprintCallable)
	void ShowTrajectoryForce(FVector Force);
	
};
