// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Enums.h>
#include "FireEngine.generated.h"

class APlayerController;
class ASpecializationCharacter;
class UInputMappingContext;
class UCurveFloat;
struct FInputActionValue;

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAtmoForce, FVector, AtmoDir, float, Magnitude);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGravityUpdate, FVector, OldGForce, FVector, NewGForce);


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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement | Reposition")
	TArray<ERepositionType> RepositionTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Throttle")
	float LateralMoveForce = 300;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Throttle")
	float MoveForce = 300;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Look")
	float TorqueForce = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Look")
	float LookDrag = 10;
	
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Reposition")
	float AngularVelocityDeterrent = .2f;*/
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Reposition")
	float RepositionTimerThreshold = 1.5f;

	float RepositionTimer = 0;

	/*UFUNCTION()
	void AdjustDirection();*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | General", meta = (MakeEditWidget))
	FVector FeetPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | General")
	float AirCheckRadius = 100;

	bool OnAir;

	bool AirChecker();

	bool IsMoving = false;

	FVector GravityForce;

	/*Under this velocity threshold the landing is easier*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Landing")
	float LandingVelocityThreshold = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Landing")
	float LandingAngularVelocityDivider = 50;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Landing")
	float LandingLinearVelocityDivider = 100;

	UFUNCTION()
	void LandHelper(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

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

	virtual void AskReposition(ERepositionType RepositionType, FVector RepositionTorqueForce, bool ForceReposition = false);
	virtual void StopReposition();

	virtual void UpdateGravityForce(FVector OldGForce, FVector NewGForce);

	bool GetOnAir() const { return OnAir; }

	// Atmosphere
	UFUNCTION(BlueprintCallable)
	void NotifyAtmoForce(bool Active);

	UPROPERTY(BlueprintAssignable)
	FOnAtmoForce OnAtmoForce;
	
	UPROPERTY(BlueprintAssignable)
	FOnGravityUpdate OnGravityUpdate;
};
