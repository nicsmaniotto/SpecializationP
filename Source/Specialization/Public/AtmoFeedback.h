// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "AtmoFeedback.generated.h"

class UMaterial;
class UMaterialInstanceDynamic;
class UCurveFloat;

/**
 * Static mesh component attached to elements for atmosphere feedback
 */
UCLASS(Blueprintable, ClassGroup = (Rendering, Common), hidecategories = (Object, Activation, "Components|Activation"), ShowCategories = (Mobility), editinlinenew, meta = (BlueprintSpawnableComponent))
class SPECIALIZATION_API UAtmoFeedback : public UStaticMeshComponent
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*Base material used to create the dynamic material instance*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback")
	UMaterial* SourceMaterial;
	
	/*Scalar value name of the dynamic material instance*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback")
	FName ScalarParameterName;

	UPROPERTY(BlueprintReadOnly)
	UMaterialInstanceDynamic* DynamicMaterial;

	/*X: current velocity; Y: scalar alpha value*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback")
	UCurveFloat* VelocityCurve;

	/*Links the VelocityCurve to the scalar value of the dynamic material instance defined by ScalarParameterName*/
	UFUNCTION(BlueprintCallable)
	void AdjustSelf(FVector Dir, float Magnitude);
	
};
