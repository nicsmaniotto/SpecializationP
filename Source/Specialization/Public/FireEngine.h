// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FireEngine.generated.h"

class APlayerController;
class ASpecializationCharacter;
class UInputMappingContext;
class UCurveFloat;
struct FInputActionValue;

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAtmoForce, FVector, AtmoDir, float, Magnitude);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPECIALIZATION_API UFireEngine : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFireEngine();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPrimitiveComponent* OwnerPhysicsComponent;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Movement
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Movement | Throttle")
	bool IsThrottling;
	UPROPERTY(BlueprintReadWrite, Category = "Movement | Throttle")
	bool IsReversing;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Throttle")
	UCurveFloat* ThrottleCurve;

	float ThrottleCurveEvaluation = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Movement | Look")
	bool IsLooking;
	
	UPROPERTY(BlueprintReadWrite, Category = "Movement | Reposition")
	bool IsRepositioning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Throttle")
	float LateralMoveForce = 300;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Throttle")
	float MoveForce = 300;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Look")
	float TorqueForce = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Look")
	float LookDrag = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Reposition")
	float AngularVelocityDeterrent = .2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Reposition")
	float RepositionTimerThreshold = 1.5f;

	float RepositionTimer = 0;

	UFUNCTION()
	void AdjustDirection();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | General", meta = (MakeEditWidget))
	FVector FeetPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | General")
	float AirCheckRadius = 100;

	bool OnAir;

	bool AirChecker();

	bool IsMoving = false;

	FVector GravityForce;

public:
	virtual void Move(const FInputActionValue& Value);
	virtual void StopMove(const FInputActionValue& Value);

	virtual void Throttle(const FInputActionValue& Value);
	virtual void EndThrottle(const FInputActionValue& Value);

	virtual void Reverse(const FInputActionValue& Value);
	virtual void StopReverse(const FInputActionValue& Value);

	virtual void VerticalMovement(float GravityMultiplier);

	virtual void StartLook(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);
	virtual void StopLook(const FInputActionValue& Value);

	virtual void AskReposition(FVector RepositionTorqueForce, bool ForceReposition = false);
	virtual void StopReposition();

	virtual void UpdateGravityForce(FVector OldGForce, FVector NewGForce);

	bool GetOnAir() const { return OnAir; }

	// Atmosphere
	UFUNCTION(BlueprintCallable)
	void NotifyAtmoForce(bool Active);

	UPROPERTY(BlueprintAssignable)
	FOnAtmoForce OnAtmoForce;
};
