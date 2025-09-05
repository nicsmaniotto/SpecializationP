// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipHUD.h"
#include "SpaceshipMovUI.h"
#include "Specialization/SpecializationCharacter.h"
#include "Components/TextBlock.h"
#include "Jetpack.h"

void USpaceshipHUD::SetupEvents_Implementation(AActor* LinkedActor)
{
	Super::SetupEvents_Implementation(LinkedActor);

	if (MovUI && MovUI->GetClass()->ImplementsInterface(USetuppable::StaticClass()))
	{
		ISetuppable::Execute_SetupEvents(MovUI->_getUObject(), LinkedActor);
	}

	ASpecializationCharacter* Player = Cast<ASpecializationCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

	if (!Player) return;

	Player->GetJetpack()->OnJetpackEquip.AddUniqueDynamic(this, &USpaceshipHUD::OnJetpackEquip);
}

void USpaceshipHUD::OnJetpackEquip(bool OnJetpack)
{
	if (OnJetpack)
	{
		ToggleJetpackTxt->SetText(UnequipTxt);
	}
	else
	{
		ToggleJetpackTxt->SetText(EquipTxt);
	}
}
