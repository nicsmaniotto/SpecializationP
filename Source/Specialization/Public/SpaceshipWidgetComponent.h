// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "SpaceshipWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALIZATION_API USpaceshipWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

	USpaceshipWidgetComponent();

protected:
	void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpaceshipWidget")
	float CloseDistance = 3000;

	UPROPERTY()
	APawn* PlayerPawn;

	bool CheckPlayerClose() const;
	
};
