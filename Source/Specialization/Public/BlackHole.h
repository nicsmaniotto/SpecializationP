// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hole.h"
#include "BlackHole.generated.h"

/**
 * Black hole implementation.
 * A white hole ref is needed to teleport to the linked location.
 */
UCLASS()
class SPECIALIZATION_API ABlackHole : public AHole
{
	GENERATED_BODY()

protected:
	virtual void OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackHole")
	class AWhiteHole* LinkedWhiteHole;
	
};
