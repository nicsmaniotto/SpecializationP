// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hole.h"
#include "Kismet/KismetMathLibrary.h"
#include "WhiteHole.generated.h"

/**
* @See Hole
 * White hole.
 */
UCLASS()
class SPECIALIZATION_API AWhiteHole : public AHole
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/*The repulsion impulse applied to elements that enters the HoleCollision range*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WhiteHole")
	float RepulsionForce = 200;
	
	/*The location an element should be placed when teleported from a black hole*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WhiteHole", meta = (MakeEditWidget))
	FTransform RepositionTransform;

	/*Array of primitive components inside the HoleCollision that should be repulsed*/
	UPROPERTY(BlueprintReadOnly, Category = "WhiteHole")
	TArray<UPrimitiveComponent*> PComponents;

public:
	UFUNCTION(BlueprintCallable)
	FTransform GetRepositionTransform() const;

};
