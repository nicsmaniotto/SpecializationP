// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include <UMG.h>
#include "Setuppable.h"
#include "BaseHUD.generated.h"

/**
 * Widget presenting the base infos an HUD has.
 */
UCLASS()
class SPECIALIZATION_API UBaseHUD : public UUserWidget, public ISetuppable
{
	GENERATED_BODY()

protected:
	/*Where to show current gravity*/
	UPROPERTY(BlueprintReadOnly, Category = "BaseHUD", meta = (BindWidget))
	UTextBlock* GravityText;
	
	/*Where to show automatic pilot ON/OFF*/
	UPROPERTY(BlueprintReadOnly, Category = "BaseHUD", meta = (BindWidget))
	UTextBlock* AutomaticPilotText;
	
	UPROPERTY(BlueprintReadOnly, Category = "BaseHUD", meta = (BindWidget))
	UCanvasPanel* AutomaticPilotContainer;

	/*ON text for automatic pilot*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseHUD")
	FText AutomaticPilotOn;
	
	/*OFF text for automatic pilot*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseHUD")
	FText AutomaticPilotOff;

	UFUNCTION()
	void OnGravityUpdate(FVector OldGravity, FVector NewGravity);

	UFUNCTION()
	void OnAutomaticPilot(bool Active);
	
	UFUNCTION()
	void OnLock(bool IsMarking);

	UPROPERTY(BlueprintReadOnly, Category = "BaseHUD", meta = (BindWidget))
	UWidget* APInputAction;

	/*Alpha when automatic pilot is not available*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseHUD", meta = (ClampMin = 0, ClampMax = 1))
	float DisabledInputAlpha = .2f;

public:
	void SetupEvents_Implementation(AActor* LinkedActor) override;
};
