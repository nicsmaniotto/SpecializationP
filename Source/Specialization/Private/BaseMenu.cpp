// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMenu.h"
#include "BaseHUD.h"

void UBaseMenu::SetupEvents(AActor* LinkedActor)
{
	if (!BaseHUD) return;

	BaseHUD->SetupEvents(LinkedActor);
}

