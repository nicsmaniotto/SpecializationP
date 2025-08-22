// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMenu.h"
#include "BaseHUD.h"


void UBaseMenu::SetupEvents_Implementation(AActor* LinkedActor)
{
	if (!BaseHUD) return;

	if (BaseHUD->GetClass()->ImplementsInterface(USetuppable::StaticClass()))
	{
		ISetuppable::Execute_SetupEvents(BaseHUD, LinkedActor);
	}
}
