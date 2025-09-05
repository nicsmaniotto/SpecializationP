// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FireEngine.h"
#include "Enums.h"
#include "Jetpack.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJetpackEquip, bool, IsEquipped);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovement, EMovType, MovType);

class UEnergyComponent;

/**
 *
 */
UCLASS()
class SPECIALIZATION_API UJetpack : public UFireEngine
{
	GENERATED_BODY()

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Propulsion")
	float AirPropulsionGrowthSpeed = .8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Propulsion")
	float GroundPropulsionGrowthSpeed = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Propulsion")
	float PropulsionDecaySpeed = 1.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Rotation")
	float CustomRepositionLerpSpeed = 65;

	UFUNCTION()
	void HandlePropSpeed(float DeltaTime);

	float PropulsionValue = 1;

	bool bCanThrottle = true;

	UPROPERTY(BlueprintReadOnly, Category = "Energy")
	UEnergyComponent* EnergyComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Energy")
	TMap<EEnergyType, bool> FuelConsumptionMap;

	EEnergyType FuelConsumptionType;

	virtual FRotator GetDirectionRotation() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bOnJetpack = false;

	UPROPERTY(BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	FVector LastMovDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MaxWalkSpeed = 1300;

	// Jump
	float JumpHoldTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* JumpForceCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float JumpTopFreedomTime = .4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float JumpTopGravityDivider = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "1"))
	float JumpDirectionMultiplier = .4f;

	float JumpTopFreedomTimer = 0;

	bool bHasJumped;

	bool IsForcedReposition;

	EMovType MovType = EMovType::NONE;

public:
	void SetDependencyComponent(UMarker* MarkerComponent, UEnergyComponent* _EnergyComponent);

	virtual void StartMove(const FInputActionValue& Value) override;

	virtual void Move(const FInputActionValue& Value) override;

	virtual void StopMove(const FInputActionValue& Value) override;

	bool HorizontalMovement(FVector2D LookAxisVector) override;

	void EndThrottle(const FInputActionValue& Value) override;

	bool VerticalMovement(float GravityMultiplier) override;

	void Reverse(const FInputActionValue& Value) override;

	void StopReverse(const FInputActionValue& Value) override;

	void UpdateGravityForce(FVector OldGForce, FVector NewGForce) override;

	virtual void AskReposition_Implementation(ERepositionType RepositionType, FVector RepositionTorqueForce, bool ForceReposition = false) override;

	bool HasPropulsion() const { return PropulsionValue > 0; }

	UFUNCTION(BlueprintCallable)
	float GetPropulsionValue() const { return PropulsionValue; }

	USceneComponent* GetRepositionableComponent_Implementation() const override;

	void CustomReposition(float DeltaTime);

	void PositionAdjustment(float DeltaTime);

	void JumpTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	bool Jump();

	UFUNCTION(BlueprintCallable)
	bool JumpOnGoing();

	UFUNCTION(BlueprintCallable)
	bool StopJumping();
	
	UFUNCTION(BlueprintCallable)
	bool ToggleJetpack();

	UFUNCTION(BlueprintCallable)
	EMovType GetMovType() const { return MovType; };

	UPROPERTY(BlueprintAssignable)
	FOnJetpackEquip OnJetpackEquip;
	
	UPROPERTY(BlueprintAssignable)
	FOnMovement OnMovement;

	UFUNCTION(BlueprintCallable)
	bool GetIsEquipped() const { return bOnJetpack; };

};
