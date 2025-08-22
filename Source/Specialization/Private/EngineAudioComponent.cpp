// Fill out your copyright notice in the Description page of Project Settings.


#include "EngineAudioComponent.h"
#include "FireEngine.h"
#include "Sound/SoundCue.h"

UEngineAudioComponent::UEngineAudioComponent()
{
	EnginePosition = EEnginePosition::CENTER;

	PrimaryComponentTick.bCanEverTick = true;
}

void UEngineAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	UFireEngine* FE = GetOwner()->GetComponentByClass<UFireEngine>();

	if (!FE)
	{
		DestroyComponent();
		return;
	}

	SetupEvents(FE);

	MaxVolume = VolumeMultiplier;

	SetVolumeMultiplier(0);
}

void UEngineAudioComponent::SetupEvents(UFireEngine* FE)
{
	switch (EnginePosition)
	{
	case EEnginePosition::CENTER:
	{
		FE->OnVerticalMovement.AddUniqueDynamic(this, &UEngineAudioComponent::OnVerticalMovement);
		FE->OnLateralMovement.AddUniqueDynamic(this, &UEngineAudioComponent::OnLateralMovement);
	}
	break;
	case EEnginePosition::TOP:
		FE->OnVerticalMovement.AddUniqueDynamic(this, &UEngineAudioComponent::OnVerticalMovement);
		break;
	case EEnginePosition::RIGHT:
	case EEnginePosition::LEFT:
		FE->OnLateralMovement.AddUniqueDynamic(this, &UEngineAudioComponent::OnLateralMovement);
		break;
	default:
		break;
	}

	FE->OnAutomaticPilot.AddUniqueDynamic(this, &UEngineAudioComponent::OnAutomaticPilot);
}

void UEngineAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float PitchM = FMath::FInterpConstantTo(PitchMultiplier, CurrentPitchMultiplier, DeltaTime, PitchInterpSpeed);

	SetPitchMultiplier(PitchM);

	float VolumeM = FMath::FInterpConstantTo(VolumeMultiplier, CurrentVolumeMultiplier, DeltaTime, VolumeInterpSpeed);

	SetVolumeMultiplier(VolumeM);
}

float UEngineAudioComponent::NormalizeForce(float Force) const
{
	//float Normalized = (Force - MinMaxForceValues.X) / (MinMaxForceValues.Y - MinMaxForceValues.X);

	return MinMaxPitchValues.X + (Force * (MinMaxPitchValues.Y - MinMaxPitchValues.X));
}

void UEngineAudioComponent::UpdateSound(float Force)
{
	float Normalized = NormalizeForce(Force);

	//CurrentPitchMultiplier = Normalized;
	CurrentPitchMultiplier = Normalized;
	
	//GEngine->AddOnScreenDebugMessage(-1, .1, FColor::Red, FString::Printf(TEXT("Adjusting pitch")));
}

bool UEngineAudioComponent::ToggleActivity(float OtherDirection, float DirectionSquaredLength)
{
	if (OtherDirection == 0)
	{
		if (CurrentVolumeMultiplier > 0 && DirectionSquaredLength == 0)
		{
			CurrentVolumeMultiplier = 0;
			CurrentPitchMultiplier = 0;
			return false;
		}

		if (CurrentVolumeMultiplier == 0 && DirectionSquaredLength > 0)
		{
			CurrentVolumeMultiplier = MaxVolume;
		}
	}

	return true;
}

void UEngineAudioComponent::OnVerticalMovement(FVector WorldDir, float Magnitude, FTransform CallingTransform)
{
	VerticalLateral.X = WorldDir.SquaredLength();

	if (!ToggleActivity(VerticalLateral.Y, VerticalLateral.X)) return;

	UpdateSound(WorldDir.Length());
}

void UEngineAudioComponent::OnLateralMovement(FVector WorldDir, float Magnitude, FTransform CallingTransform)
{
	FVector EngineRelativeDir = GetComponentLocation() - GetOwner()->GetActorLocation();
	float DotValue = FVector::DotProduct(WorldDir.GetSafeNormal(), EngineRelativeDir.GetSafeNormal());

	if (DotValue < LateralMovementDotAcceptance)
	{
		VerticalLateral.Y = WorldDir.SquaredLength();
	}
	else
	{
		VerticalLateral.Y = 0;
	}

	if (!ToggleActivity(VerticalLateral.X, VerticalLateral.Y)) return;

	UpdateSound(WorldDir.Length());
}

void UEngineAudioComponent::OnAutomaticPilot(bool IsActive)
{
	if (!IsActive)
	{
		UpdateSound(0);
	}
}
