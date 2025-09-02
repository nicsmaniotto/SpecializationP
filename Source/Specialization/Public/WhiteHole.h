// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hole.h"
#include "Kismet/KismetMathLibrary.h"
#include "WhiteHole.generated.h"

/**
 * 
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WhiteHole")
	float RepulsionForce = 200;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WhiteHole", meta = (MakeEditWidget))
	FVector RepositionLocation;

	UPROPERTY(BlueprintReadOnly, Category = "WhiteHole")
	TArray<UPrimitiveComponent*> PComponents;

public:
	UFUNCTION(BlueprintCallable)
	FVector GetRepositionLocation() const { return UKismetMathLibrary::TransformLocation(GetActorTransform(), RepositionLocation); }

};
