// Fill out your copyright notice in the Description page of Project Settings.


#include "OxygenZone.h"
#include "EnergyComponent.h"

void UOxygenZone::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddUniqueDynamic(this, &UOxygenZone::OnBeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UOxygenZone::OnEndOverlap);
}

void UOxygenZone::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UEnergyComponent* EC = Cast<UEnergyComponent>(OtherActor->GetComponentByClass<UEnergyComponent>());

	if(!EC || !EC->HasEnergyType(EEnergyType::OXYGEN)) return;

	EC->RestoreMaxEnergy(EEnergyType::OXYGEN, true);
}

void UOxygenZone::OnEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UEnergyComponent* EC = Cast<UEnergyComponent>(OtherActor->GetComponentByClass<UEnergyComponent>());

	if (!EC || !EC->HasEnergyType(EEnergyType::OXYGEN)) return;

	EC->RestoreMaxEnergy(EEnergyType::OXYGEN, false);
}
