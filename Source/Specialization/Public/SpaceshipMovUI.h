// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Setuppable.h"
#include "Components/ProgressBar.h"
#include <Enums.h>
#include "SpaceshipMovUI.generated.h"

/**
 * Widget that visualizes the spaceship movement
 */
UCLASS()
class SPECIALIZATION_API USpaceshipMovUI : public UUserWidget, public ISetuppable
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	UProgressBar* TopProgressBar;

	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	UProgressBar* LeftProgressBar;

	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	UProgressBar* RightProgressBar;

	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	UProgressBar* FrontProgressBar;

	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	UProgressBar* BackProgressBar;

	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	UProgressBar* BottomProgressBar;

protected:
	virtual void NativeConstruct() override;

	virtual void SetupEvents_Implementation(AActor* LinkedActor) override;

	UFUNCTION()
	void OnVerticalMovement(FVector WorldDir, float Magnitude, FTransform CallingTransform);

	UFUNCTION()
	void OnLateralMovement(FVector WorldDir, float Magnitude, FTransform CallingTransform);
	
	UFUNCTION()
	void OnAutomaticPilot(bool Active);
	
	UFUNCTION()
	void OnSpaceshipInteraction(bool OnSpaceship);

	void ResetPercentages();

	TMap<EBarPosition, UProgressBar*> BarPositions;

private:
	UProgressBar* GetOpposingBar(EBarPosition Position) const;

	void FillBar(EBarPosition Position, float Perc);
	
};
