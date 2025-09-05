// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Enums.h>
#include "Repositionable.h"
#include "FireEngine.generated.h"

class APlayerController;
class ASpecializationCharacter;
class UInputMappingContext;
class UCurveFloat;
class APlanet;
class UMarker;
struct FInputActionValue;

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAtmoForce, FVector, AtmoDir, float, Magnitude);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGravityUpdate, FVector, OldGForce, FVector, NewGForce);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAutomaticPilot, bool, Active);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnVerticalMovement, FVector, ForceDir, float, Magnitude, FTransform, Transform);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLateralMovement, FVector, ForceDir, float, Magnitude, FTransform, Transform);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPECIALIZATION_API UFireEngine : public UActorComponent, public IRepositionable
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
	
	UFUNCTION(BlueprintCallable, Category = "Movement | Look")
	virtual FRotator GetDirectionRotation() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Reposition")
	float RepositionTimerThreshold = 1.5f;

	float RepositionTimer = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | General", meta = (MakeEditWidget))
	FVector FeetPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | General")
	float AirCheckLength = 100;

	bool OnAir;

	bool AirChecker();

	//bool IsMoving = false;

	FVector GravityForce;

	/*Under this velocity threshold the landing is easier*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Landing")
	float LandingVelocityThreshold = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Landing")
	float LandingAngularVelocityDivider = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Landing")
	float LandingLinearVelocityDivider = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Damping")
	float SpaceLinearDamping = .1f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement | Damping")
	float NormalLinearDamping;

	UFUNCTION()
	void LandHelper(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	APlanet* LandingPlanet;

	/*Automatic pilot*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Automatic")
	float AutomaticApproachAcceptance = 10;

	UPROPERTY(BlueprintReadOnly)
	FHitResult SurfaceHit;

	bool IsRetroFireActivated;


public:
	virtual void StartMove(const FInputActionValue& Value) {};
	virtual void Move(const FInputActionValue& Value);
	virtual void StopMove(const FInputActionValue& Value);

	virtual void Throttle(const FInputActionValue& Value);
	virtual void EndThrottle(const FInputActionValue& Value);

	virtual void Reverse(const FInputActionValue& Value);
	virtual void StopReverse(const FInputActionValue& Value);

	virtual bool HorizontalMovement(FVector2D LookAxisVector);
	virtual bool VerticalMovement(float GravityMultiplier);

	virtual void StartLook(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);
	virtual void StopLook(const FInputActionValue& Value);

	virtual void AskReposition_Implementation(ERepositionType RepositionType, FVector RepositionTorqueForce, bool ForceReposition = false) override;
	virtual void StopReposition();

	virtual USceneComponent* GetRepositionableComponent_Implementation() const override { return OwnerPhysicsComponent; }

	virtual void UpdateGravityForce(FVector OldGForce, FVector NewGForce);

	virtual void ToggleAutomaticPilot(bool Active);

	virtual void AutomaticPilotMovement();

	bool IsAutomatic;

	bool GetIsAutomaticPilot() const { return IsAutomatic; }

	UFUNCTION(BlueprintCallable)
	bool GetOnAir() const { return OnAir; }

	// Functionality
	virtual void SetDependencyComponent(UMarker* MarkerComponent);

	// Atmosphere
	UFUNCTION(BlueprintCallable)
	FVector GetGForce() const { return GravityForce; }

	UFUNCTION(BlueprintCallable)
	bool IsInAtmosphere() const { return GravityForce.SquaredLength() > 0; }

	UFUNCTION(BlueprintCallable)
	void NotifyAtmoForce(bool Active);

	UPROPERTY(BlueprintAssignable)
	FOnAtmoForce OnAtmoForce;

	UPROPERTY(BlueprintAssignable)
	FOnGravityUpdate OnGravityUpdate;

	UFUNCTION(BlueprintCallable)
	APlanet* GetPlanetSurface() const { return LandingPlanet; };

	UFUNCTION(BlueprintCallable)
	FHitResult GetHitSurface() const { return SurfaceHit; }

public:
	// Automatic pilot
	UPROPERTY(BlueprintAssignable)
	FOnAutomaticPilot OnAutomaticPilot;

	// Movement
	UPROPERTY(BlueprintAssignable)
	FOnVerticalMovement OnVerticalMovement;

	UPROPERTY(BlueprintAssignable)
	FOnLateralMovement OnLateralMovement;

	// Dependecies
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Dependencies")
	UMarker* Marker;
};
