// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "Enums.h"
#include "EngineAudioComponent.generated.h"

/**
 * Audio component for an actor that presents a fire engine.
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

	/*
	* Its relative position.
	* Defines the strength of the audio volume based on its position and the fire engine movement.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EngineAudio")
	EEnginePosition EnginePosition;
	
	/*The possible range of the pitch value of the sound*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EngineAudio")
	FVector2D MinMaxPitchValues;

	/*Pitch interpolation speed*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EngineAudio")
	float PitchInterpSpeed = .6f;
	
	/*Volume interpolation speed*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EngineAudio")
	float VolumeInterpSpeed = .3f;

	float MaxVolume;

	/*Only for CENTER, LEFT, RIGHT*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EngineAudio")
	float LateralMovementDotAcceptance = .2f;

	UFUNCTION(BlueprintCallable)
	float NormalizeForce(float Force) const;

	UFUNCTION()
	void OnVerticalMovement(FVector WorldDir, float Magnitude, FTransform CallingTransform);

	UFUNCTION()
	void OnLateralMovement(FVector WorldDir, float Magnitude, FTransform CallingTransform);

	UFUNCTION()
	FVector GetRelativeDir() const;
	
	UFUNCTION()
	void OnAutomaticPilot(bool Active);

	void UpdateSound(float Force);

	/*
	* Checks whether fire engine current force (opposing direction) is active.
	*/
	bool ToggleActivity(float OpposingDirection, float DirectionSquaredLength);

	FVector2D VerticalLateral;

	float CurrentPitchMultiplier = 0;

	float CurrentVolumeMultiplier = 0;
};
