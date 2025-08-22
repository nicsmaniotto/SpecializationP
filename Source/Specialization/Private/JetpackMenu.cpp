// Fill out your copyright notice in the Description page of Project Settings.


#include "JetpackMenu.h"
#include "UMG.h"
#include "Specialization/SpecializationCharacter.h"

void UJetpackMenu::NativeConstruct()
{
	Super::NativeConstruct();
}

void UJetpackMenu::SetupEvents_Implementation(AActor* LinkedActor)
{
	Super::SetupEvents_Implementation(LinkedActor);

	ASpecializationCharacter* Player = Cast<ASpecializationCharacter>(LinkedActor);

	if (!Player) return;

	Player->OnJetpackEquip.AddUniqueDynamic(this, &UJetpackMenu::OnJetpackEquip);
}

void UJetpackMenu::OnJetpackEquip(bool IsEquipped)
{
	if (!JetpackContainer) return;

	if (AppearAnim)
	{
		if (IsEquipped)
		{
			PlayAnimationForward(AppearAnim);
			return;
		}

		PlayAnimationReverse(AppearAnim);
	}

}
