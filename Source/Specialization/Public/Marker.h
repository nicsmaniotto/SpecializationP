// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Marker.generated.h"

class UMarkingComponent;
class APlayerController;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPECIALIZATION_API UMarker : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMarker();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Marker")
	float DotMinThreshold = .9f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Marker")
	float CheckDistance = 2000000;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Marker")
	TEnumAsByte<ETraceTypeQuery> CheckType;

	UFUNCTION(BlueprintCallable)
	APlayerController* CheckController();
	
	UFUNCTION(BlueprintCallable)
	UMarkingComponent* CheckMarker(APlayerController* PlayerController);

	UPROPERTY(BlueprintReadOnly, Category = "Marker")
	UMarkingComponent* MarkedObject;

	UPROPERTY(BlueprintReadOnly, Category = "Marker")
	bool IsMarking;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	UMarkingComponent* GetMarkedObject() const { return MarkedObject; }
	
	UFUNCTION(BlueprintCallable)
	bool GetIsMarking() const { return IsMarking; }
	
	UFUNCTION(BlueprintCallable)
	void ToggleMarkObject();
	
	UFUNCTION(BlueprintCallable)
	bool ToggleTrajectory();

	UFUNCTION(BlueprintCallable)
	FTransform GetSelfTransform() const;

	UFUNCTION(BlueprintCallable)
	void ToggleSelf(bool Active);
};
