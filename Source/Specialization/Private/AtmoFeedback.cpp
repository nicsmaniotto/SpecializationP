// Fill out your copyright notice in the Description page of Project Settings.


#include "AtmoFeedback.h"
#include <FireEngine.h>
#include <Kismet/KismetMathLibrary.h>

void UAtmoFeedback::BeginPlay()
{
	Super::BeginPlay();

	UFireEngine* FE = GetOwner()->GetComponentByClass<UFireEngine>();

	if (!FE)
	{
		DestroyComponent();
		return;
	}

	FE->OnAtmoForce.AddUniqueDynamic(this, &UAtmoFeedback::AdjustSelf);

	DynamicMaterial = CreateDynamicMaterialInstance(0, SourceMaterial);
}

void UAtmoFeedback::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAtmoFeedback::AdjustSelf(FVector Dir, float Magnitude)
{
	if (!VelocityCurve || !DynamicMaterial) return;

	SetHiddenInGame(Magnitude == 0);

	if (Magnitude == 0) return;

	// adjust intensity
	float Val = VelocityCurve->GetFloatValue(Magnitude);
	DynamicMaterial->SetScalarParameterValue(ScalarParameterName, Val);

	GEngine->AddOnScreenDebugMessage(-1, .1, FColor::White, FString::Printf(TEXT("Value: %f"), Val));

	// adjust direction
	FRotator r = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), GetComponentLocation() + Dir);

	SetWorldRotation(r);
}
