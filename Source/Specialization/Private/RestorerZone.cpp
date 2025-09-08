// Fill out your copyright notice in the Description page of Project Settings.


#include "RestorerZone.h"
#include "EnergyComponent.h"

URestorerZone::URestorerZone()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URestorerZone::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddUniqueDynamic(this, &URestorerZone::OnBeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &URestorerZone::OnEndOverlap);
}

void URestorerZone::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (UEnergyComponent* EC : EnergyComponents)
	{
		EC->RestoreMaxEnergy(RestorerType, true);
	}
}

void URestorerZone::OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UEnergyComponent* EC = Cast<UEnergyComponent>(OtherActor->GetComponentByClass<UEnergyComponent>());

	if (!EC || !EC->HasEnergyType(RestorerType)) return;

	EnergyComponents.Add(EC);
}

void URestorerZone::OnEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UEnergyComponent* EC = Cast<UEnergyComponent>(OtherActor->GetComponentByClass<UEnergyComponent>());

	if (!EC || !EC->HasEnergyType(RestorerType)) return;

	EC->RestoreMaxEnergy(RestorerType, false);

	EnergyComponents.Remove(EC);
}
