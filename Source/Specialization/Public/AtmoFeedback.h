// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "AtmoFeedback.generated.h"

class UMaterial;
class UMaterialInstanceDynamic;
class UCurveFloat;

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Rendering, Common), hidecategories = (Object, Activation, "Components|Activation"), ShowCategories = (Mobility), editinlinenew, meta = (BlueprintSpawnableComponent))
class SPECIALIZATION_API UAtmoFeedback : public UStaticMeshComponent
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback")
	UMaterial* SourceMaterial;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback")
	FName ScalarParameterName;

	UPROPERTY(BlueprintReadOnly)
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Feedback")
	UCurveFloat* VelocityCurve;

	UFUNCTION(BlueprintCallable)
	void AdjustSelf(FVector Dir, float Magnitude);
	
};
