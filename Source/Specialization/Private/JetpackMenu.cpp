// Fill out your copyright notice in the Description page of Project Settings.


#include "JetpackMenu.h"
#include "UMG.h"
#include "FireEngine.h"
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
		Player->OnJetpackEquip.AddUniqueDynamic(this, &UJetpackMenu::OnJetpackEquip);
	}

	UFireEngine* FE = LinkedActor->GetComponentByClass<UFireEngine>();

	if (FE)
	{
		FE->OnGravityUpdate.AddUniqueDynamic(this, &UJetpackMenu::OnGravityUpdate);
	}
}

void UJetpackMenu::OnJetpackEquip(bool IsEquipped)
{
	if (!JetpackContainer) return;

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

void UJetpackMenu::OnGravityUpdate(FVector OldGravity, FVector NewGravity)
{
	if (!IsJetpackEquipped) return;

	if (PropulsionSlider->GetRenderOpacity() == 0 && NewGravity.SquaredLength() > 0)
	{
		PlayAnimationForward(PropulsionAppearAnim);
	}

	if (PropulsionSlider->GetRenderOpacity() > 0 && NewGravity.SquaredLength() == 0)
	{
		PlayAnimationReverse(PropulsionAppearAnim);
	}
}
