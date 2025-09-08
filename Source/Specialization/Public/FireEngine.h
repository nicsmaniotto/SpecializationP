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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAtmosphereUpdate, APlanet*, Planet);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnVerticalMovement, FVector, ForceDir, float, Magnitude, FTransform, Transform);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLateralMovement, FVector, ForceDir, float, Magnitude, FTransform, Transform);

/*
* This actor component is a reusable component for every object we want to be moved on air.
* This actor manages:
*	- on air movement / rotation
* 	- gravity 
* 	- automatic pilot.
*/
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

	/*
	* Curve of force multiplied to throttle MoveForce when owner is not on air.
	* It can be seen as a detach force from terrain.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Throttle")
	UCurveFloat* ThrottleCurve;

	float ThrottleCurveEvaluation = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Movement | Look")
	bool IsLooking;

	UPROPERTY(BlueprintReadWrite, Category = "Movement | Reposition")
	bool IsRepositioning;

	/*
	* @see class: GravityBound - function: AskAlignment
	* @see class: this - function: UpdateGravityForce
	* Allowable reposition types an atmosphere can execute on the owner
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement | Reposition")
	TArray<ERepositionType> RepositionTypes;

	/*
	* Force for relative XY movement
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Throttle")
	float LateralMoveForce = 300;

	/*
	* Force for relative Z movement
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Throttle")
	float MoveForce = 300;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Look")
	float TorqueForce = 10;
	
	/*
	* Gets the rotation of the component used for determining the movement direction
	*/
	UFUNCTION(BlueprintCallable, Category = "Movement | Look")
	virtual FRotator GetDirectionRotation() const;

	/*
	* Timer that determines when to execute the reposition (if not forced)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Reposition")
	float RepositionTimerThreshold = 1.5f;

	float RepositionTimer = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | General", meta = (MakeEditWidget))
	FVector FeetPosition;

	/*
	* Distance of the line trace to determine if on air or not
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | General")
	float AirCheckLength = 100;

	bool OnAir;

	/*
	* function to determine if on air or not (placed on tick)
	*/
	bool AirChecker();

	/*
	* @see class: GravityBound - function: ExecuteGravity
	* @see class: this - function: 
	* Stored gravity force
	*/
	FVector GravityForce;

	/*Under this velocity threshold, landing is easier*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Landing")
	float LandingVelocityThreshold = 10000;
	
	/* Damping used when in space */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Damping")
	float SpaceLinearDamping = .1f;
	
	/* Damping used when in atmosphere (or in automatic pilot) */
	UPROPERTY(BlueprintReadOnly, Category = "Movement | Damping")
	float NormalLinearDamping;

	/* used on hit to reduce instantaneously velocity */
	UFUNCTION()
	void LandHelper(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	APlanet* LandingPlanet;

	/*Automatic pilot approach distance acceptance*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Automatic")
	float AutomaticApproachAcceptance = 10;
	
	/*This value is an adjustment value for the retrieval of the distance to stop. Higher the value, closer to object*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement | Automatic")
	float StopForceAdjustmentValue = .35f;

	/*Useful as a variable for sound*/
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

	/*
	* @see class: GravityBound - function: ExecuteGravity
	*/
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
	bool IsInAtmosphere() const { return !!LandingPlanet; }

	UPROPERTY(BlueprintAssignable)
	FOnAtmosphereUpdate OnAtmosphereUpdate;

	// notifies liminal atmosphere force
	// @see class AtmoFeedback
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
