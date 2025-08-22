// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "Enums.h"
#include "EngineAudioComponent.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALIZATION_API UEngineAudioComponent : public UAudioComponent
{
	GENERATED_BODY()

	UEngineAudioComponent();

protected:
	virtual void BeginPlay() override;

	void SetupEvents(class UFireEngine* FE);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EngineAudio")
	EEnginePosition EnginePosition;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EngineAudio")
	FVector2D MinMaxForceValues;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EngineAudio")
	FVector2D MinMaxPitchValues;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EngineAudio")
	float PitchInterpSpeed = .6f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EngineAudio")
	float VolumeInterpSpeed = .3f;

	float MaxVolume;

	/*Only for CENTER, LEFT, RIGHT*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EngineAudio")
	float LateralMovementDotAcceptance = .2f;
	

	UFUNCTION(BlueprintCallable)
	float NormalizeForce(float Force) const;

	FVector SelfRelativePosition;

	UFUNCTION()
	void OnVerticalMovement(FVector WorldDir, float Magnitude, FTransform CallingTransform);

	UFUNCTION()
	void OnLateralMovement(FVector WorldDir, float Magnitude, FTransform CallingTransform);
	
	UFUNCTION()
	void OnAutomaticPilot(bool Active);

	void UpdateSound(float Force);

	bool ToggleActivity(float OtherDirection, float DirectionSquaredLength);

	FVector2D VerticalLateral;

	float CurrentPitchMultiplier = 0;

	float CurrentVolumeMultiplier = 0;
};
