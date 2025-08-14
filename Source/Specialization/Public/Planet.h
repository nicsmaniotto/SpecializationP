// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Markable.h"
#include "Planet.generated.h"

class UGravityBound;
class USplineComponent;

UCLASS()
class SPECIALIZATION_API APlanet : public AActor, public IMarkable
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Category = "Planet | Structure", BlueprintReadOnly)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, Category = "Planet | Structure", BlueprintReadOnly)
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(VisibleAnywhere, Category = "Planet | Structure", BlueprintReadOnly)
	class UMarkingComponent* MarkingComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Planet | Structure", BlueprintReadOnly)
	UGravityBound* GravityBound;
	
	UPROPERTY(VisibleAnywhere, Category = "Planet | Structure", BlueprintReadOnly)
	USplineComponent* Orbit;

	// SelfRotation
	UPROPERTY(EditAnywhere, Category = "Planet | Generic", BlueprintReadOnly)
	float RotationSpeed = .2f;

	UFUNCTION(BlueprintCallable)
	void MeshRotation(float DeltaTime);

	// Orbit
	UPROPERTY(EditAnywhere, Category = "Planet | Orbit", BlueprintReadOnly)
	float OrbitSpeed = 1200;

	UFUNCTION(BlueprintCallable)
	void OrbitFollow(float DeltaTime);

	float PositionInSpline = 0;
	
public:	
	// Sets default values for this actor's properties
	APlanet();

	USceneComponent* GetMarkedObject_Implementation() override;

	UStaticMeshComponent* GetMesh() const { return Mesh; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetupChildren();

	FVector LastLocation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetDeltaVelocity() const;

	FVector GetDeltaAngForce(FVector Location) const;

};
