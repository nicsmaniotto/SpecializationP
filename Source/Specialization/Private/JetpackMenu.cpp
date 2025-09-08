// Fill out your copyright notice in the Description page of Project Settings.


#include "JetpackMenu.h"
#include "UMG.h"
#include "FireEngine.h"
#include "Jetpack.h"
#include "Specialization/SpecializationCharacter.h"

void UJetpackMenu::NativeConstruct()
{
	Super::NativeConstruct();
}

void UJetpackMenu::SetupEvents_Implementation(AActor* LinkedActor)
{
	Super::SetupEvents_Implementation(LinkedActor);

	ASpecializationCharacter* Player = Cast<ASpecializationCharacter>(LinkedActor);

	if (Player)
	{
		Player->GetJetpack()->OnJetpackEquip.AddUniqueDynamic(this, &UJetpackMenu::OnJetpackEquip);
	}

	UFireEngine* FE = LinkedActor->GetComponentByClass<UFireEngine>();

	if (FE)
	{
		FE->OnAtmosphereUpdate.AddUniqueDynamic(this, &UJetpackMenu::OnAtmosphereUpdate);
	}
}

void UJetpackMenu::OnJetpackEquip(bool IsEquipped)
{
	IsJetpackEquipped = IsEquipped;

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

void UJetpackMenu::OnAtmosphereUpdate(APlanet* Planet)
{
	if (!IsJetpackEquipped) return;

	if (PropulsionSlider->GetRenderOpacity() == 0 && !!Planet)
	{
		PlayAnimationForward(PropulsionAppearAnim);
	}

	if (PropulsionSlider->GetRenderOpacity() > 0 && !Planet)
	{
		PlayAnimationReverse(PropulsionAppearAnim);
	}
}
