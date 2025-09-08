// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Enums.h"
#include "EnergyRestorerInteractable.generated.h"

/**
 * Zone where an EnergyComponent with a specific RestorerType will be refilled upon interacting
 */
UCLASS()
class SPECIALIZATION_API AEnergyRestorerInteractable : public AActor, public IInteractable
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	class UInteractableComponent* InteractableComponent;
	
public:	
	// Sets default values for this actor's properties
	AEnergyRestorerInteractable();

protected:
	UFUNCTION(BlueprintCallable)
	void RestoreEnergy(AActor* OtherActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
	EEnergyType RestorerType;

public:	

	void Interact_Implementation(class ASpecializationCharacter* Player) override;

};
