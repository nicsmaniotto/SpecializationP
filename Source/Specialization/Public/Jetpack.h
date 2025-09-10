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
 * @See class FireEngine
 * This component works both as fire engine and movement component for player.
 * Moreover, the alignment from a gravity force is overriden into a more controllable component
 * retrieved by the owner (if implements IRepositionable).
 * @See interface IRepositionable
 */
UCLASS()
class SPECIALIZATION_API UJetpack : public UFireEngine
{
	GENERATED_BODY()

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*Alignment "override" in order to always follow gravity alignment without physics forces*/
	virtual void CustomReposition(float DeltaTime);

	/*Compensation for planet movement*/
	virtual void PositionAdjustment(float DeltaTime);

	/*Jump behavior*/
	virtual void JumpTick(float DeltaTime);

protected:
	/*Propulsion growth value on air*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Propulsion")
	float AirPropulsionGrowthSpeed = .8f;

	/*Propulsion growth value on land*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Propulsion")
	float GroundPropulsionGrowthSpeed = 2;

	/*Propulsion decay value when used*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Propulsion")
	float PropulsionDecaySpeed = 1.2f;

	/*Manages propulsion*/
	UFUNCTION()
	void HandlePropSpeed(float DeltaTime);

	float PropulsionValue = 1;

	/*Alignment rotation lerp value*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Rotation")
	float CustomRepositionLerpSpeed = 65;

	bool bCanThrottle = true;

	UPROPERTY(BlueprintReadOnly, Category = "Energy")
	UEnergyComponent* EnergyComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Energy")
	TMap<EEnergyType, bool> FuelConsumptionMap;

	EEnergyType FuelConsumptionType;

	virtual FRotator GetDirectionRotation() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Walk", meta = (AllowPrivateAccess = "true"))
	bool bOnJetpack = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement | Walk", meta = (AllowPrivateAccess = "true"))
	FVector LastMovDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Walk", meta = (AllowPrivateAccess = "true"))
	float MaxWalkSpeed = 1300;

	// Jump
	/*X: time; Y: Applied relative Z Force*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Walk", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* JumpForceCurve;

	float JumpHoldTimer;

	/*Custom top gravity timer*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Walk", meta = (AllowPrivateAccess = "true"))
	float JumpTopFreedomTime = .4f;

	float JumpTopFreedomTimer = 0;

	/*Custom top gravity divider: higher the value, less oppressive is gravity*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Walk", meta = (AllowPrivateAccess = "true"))
	float JumpTopGravityDivider = 1.5f;

	/*Scale value of the movement direction applied to jump*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Walk", meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "1"))
	float JumpDirectionMultiplier = .4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Walk", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ETraceTypeQuery> StopWalkTypesCheck = UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Walk", meta = (AllowPrivateAccess = "true"))
	float StopWalkCheckOffset = 50;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement | Walk", meta = (AllowPrivateAccess = "true"))
	float StopWalkCheckLength = 50;

	bool bHasJumped;

	bool IsForcedReposition;

	/*Current Movement state*/
	EMovType MovType = EMovType::NONE;

public:
	void SetDependencyComponent(UMarker* MarkerComponent, UEnergyComponent* _EnergyComponent);

	virtual void StartMove(const FInputActionValue& Value) override;

	virtual void Move(const FInputActionValue& Value) override;

	virtual bool WalkMoveCheck(FVector& MovementDir);

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
