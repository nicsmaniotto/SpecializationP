// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseHUD.h"
#include <FireEngine.h>
#include <Marker.h>
#include <Specialization/SpecializationGameMode.h>

void UBaseHUD::SetupEvents_Implementation(AActor* LinkedActor)
{
	if (!LinkedActor) return;

	UFireEngine* FE = LinkedActor->GetComponentByClass<UFireEngine>();

	if (FE)
	{
		FE->OnGravityUpdate.AddUniqueDynamic(this, &UBaseHUD::OnGravityUpdate);
		FE->OnAutomaticPilot.AddUniqueDynamic(this, &UBaseHUD::OnAutomaticPilot);
	}

	UMarker* Marker = LinkedActor->GetComponentByClass<UMarker>();

	if (Marker)
	{
		Marker->OnLock.AddUniqueDynamic(this, &UBaseHUD::OnLock);
	}

}

void UBaseHUD::OnGravityUpdate(FVector OldGravity, FVector NewGravity)
{
	AutomaticPilotContainer->SetVisibility(NewGravity.SquaredLength() == 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);

	float GravityLength = NewGravity.Length() / ASpecializationGameMode::GRAVITYMULTIPLIER;

	float Decimals = GravityLength - (int)GravityLength;
	Decimals *= 10;
	Decimals = FMath::TruncToInt(Decimals);

	FString str = FString::Printf(TEXT("%d,%d"), (int)GravityLength, (int)Decimals);

	GravityText->SetText(FText::FromString(str));
}

void UBaseHUD::OnAutomaticPilot(bool Active)
{
	AutomaticPilotText->SetText(Active ? AutomaticPilotOn : AutomaticPilotOff);
}

void UBaseHUD::OnLock(bool IsMarking)
{
	APInputAction->SetRenderOpacity(IsMarking ? 1 : DisabledInputAlpha);
}
