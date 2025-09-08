// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyRestorerInteractable.h"
#include "EnergyComponent.h"
#include "InteractableComponent.h"
#include "Specialization/SpecializationCharacter.h"

// Sets default values
AEnergyRestorerInteractable::AEnergyRestorerInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable Component"));
	SetRootComponent(InteractableComponent);
}

void AEnergyRestorerInteractable::Interact_Implementation(ASpecializationCharacter* Player)
{
	RestoreEnergy(Player);
}

void AEnergyRestorerInteractable::RestoreEnergy(AActor* OtherActor)
{
	UEnergyComponent* EC = Cast<UEnergyComponent>(OtherActor->GetComponentByClass<UEnergyComponent>());

	if (!EC || !EC->HasEnergyType(RestorerType)) return;

	EC->RestoreMaxEnergy(RestorerType, true);
}
