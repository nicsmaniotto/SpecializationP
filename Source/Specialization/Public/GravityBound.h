// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "GravityBound.generated.h"

class UCurveFloat;
class UFireEngine;

/**
 * 
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

	/*X = Distance; Y = AppliedForce*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gravity")
	UCurveFloat* GravityCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gravity")
	float RedirectionForce = 100;
	
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
	void AskAlignement(UPrimitiveComponent* PrimitiveComponent, UFireEngine* FireEngine, FVector Dir);

	UPROPERTY()
	TMap<UFireEngine*, FVector> LastGForces;

	/*Atmosphere vel change*/
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool ShowAtmoInner = true;

	void AtmosphereVelChange(UPrimitiveComponent* PrimitiveComponent, UFireEngine* FireEngine, FVector Dir);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	bool HasAtmoVelChange = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float MinAtmoVelocity = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float AtmoCheckRadius = 500;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float AtmoCheckHeight = 500;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float AtmoDotValue = .2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float VelChangeValue = 15000;

public:
	virtual void EnlistComponent(UPrimitiveComponent* OtherComp);

	virtual void UnenlistComponent(UPrimitiveComponent* OtherComp);
};
