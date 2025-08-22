// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceshipHUD.h"
#include "SpaceshipMovUI.h"

void USpaceshipHUD::SetupEvents_Implementation(AActor* LinkedActor)
{
	Super::SetupEvents_Implementation(LinkedActor);

	if (MovUI && MovUI->GetClass()->ImplementsInterface(USetuppable::StaticClass()))
	{
		ISetuppable::Execute_SetupEvents(MovUI->_getUObject(), LinkedActor);
	}
}
