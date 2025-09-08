// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Enums.h"
#include "GravityBound.generated.h"

class UCurveFloat;
class UFireEngine;

/**
 * This capsule component child class consists of an actual gravity manager for an object.
 * It manages the calculation of:
 *	- gravity,
 * 	- alignment to gravity direction,
 * 	- liminal atmosphere forces.
 */
UCLASS(Blueprintable, BlueprintType, editinlinenew, hidecategories = (Object, LOD, Lighting, TextureStreaming), meta = (DisplayName = "Gravity Bound", BlueprintSpawnableComponent))
class SPECIALIZATION_API UGravityBound : public UCapsuleComponent
{
	GENERATED_BODY()

protected:
	UGravityBound();

	void BeginPlay() override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gravity", meta = (MakeEditWidget))
	FVector CenterOfGravity;

	/*
	* X = Distance; Y = AppliedForce
	* Y is multiplied by a scale factor defined in the game mode
	* @see class SpecializationGameMode
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gravity")
	UCurveFloat* GravityCurve;
	
	/*
	* Normal alignment is with the downvector of the actor facing the direction of the gravity
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gravity")
	float RedirectionForce = 320;
	
	/*
	* whether the alignment should be forced istantaneously
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gravity")
	bool ForceReposition = false;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(BlueprintReadWrite, Category = "Gravity")
	TArray< UPrimitiveComponent*> Overlaps;

	/*Returns the gravity direction*/
	FVector ExecuteGravity(UPrimitiveComponent* PrimitiveComponent, UFireEngine* FireEngine);

	/*Acknowledges the fire engine involved of how to alignate with the gravity axis*/
	void AskAlignement(USceneComponent* PrimitiveComponent, UFireEngine* FireEngine, FVector Dir);

	/*Returns the gravity direction for the specific fire engine*/
	UPROPERTY()
	TMap<UFireEngine*, FVector> LastGForces;

	/*Atmosphere velocity change*/
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool ShowAtmoInner = true;

	/*
	* Executes liminal atmosphere behavior
	*/
	void AtmosphereVelChange(UPrimitiveComponent* PrimitiveComponent, UFireEngine* FireEngine, FVector Dir);

	/*
	* Should the Alignment be reversed?
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	bool bReverseAlignmentDir = false;
	
	/*
	* Should the Alignment face the gravity direction?
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	bool bFaceDir = false;
	
	/*
	* Kind of alignments this atmosphere presents
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	TArray<ERepositionType> AllowedRepositionTypes = { ERepositionType::FORWARD, ERepositionType::RIGHT };
	
	/*
	* Should the atmosphere present a liminal atmosphere behavior?
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	bool HasAtmoVelChange = true;

	/*
	* Minimum velocity of the fire engine for liminal atmosphere behavior change
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float MinAtmoVelocity = 1000;

	/*
	* Outside this radius the liminal atmosphere starts
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float AtmoCheckRadius = 500;
	
	/*
	* Outside this half height the liminal atmosphere starts
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float AtmoCheckHeight = 500;
	
	/*
	* Dot value between fire engine velocity and gravity that represents the check for increasing of the velocity.
	* This value is used as a acceptance threshold for the perpendicularity of the dotproduct (if dotproduct is 0 it is perpendicular)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float AtmoDotValue = .2f;

	/*
	* Liminal atmosphere velocity increment
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float VelChangeValue = 15000;

public:
	virtual void EnlistComponent(UPrimitiveComponent* OtherComp);

	virtual void UnenlistComponent(UPrimitiveComponent* OtherComp);
};
