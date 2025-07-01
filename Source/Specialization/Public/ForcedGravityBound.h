// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GravityBound.h"
#include "ForcedGravityBound.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, editinlinenew, hidecategories = (Object, LOD, Lighting, TextureStreaming), meta = (DisplayName = "Forced Gravity Bound", BlueprintSpawnableComponent))
class SPECIALIZATION_API UForcedGravityBound : public UGravityBound
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Forced")
	TArray<AActor*> Outers;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Forced")
	float LinearVelocityAdjusted = 300;

public:
	void EnlistComponent(UPrimitiveComponent* OtherComp) override;

	void UnenlistComponent(UPrimitiveComponent* OtherComp) override;
	
};
