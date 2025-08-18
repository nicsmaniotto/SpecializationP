// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Markable.h"
#include "Enums.h"
#include "MarkingComponent.generated.h"

class UMarker;
class UMarkerWidget;

/**
 *
 */
UCLASS(Blueprintable, ClassGroup = "UserInterface", hidecategories = (Object, Activation, "Components|Activation", Sockets, Base, Lighting, LOD, Mesh), editinlinenew, meta = (BlueprintSpawnableComponent))
class SPECIALIZATION_API UMarkingComponent : public UWidgetComponent
{
	GENERATED_BODY()

	UMarkingComponent();

protected:
	void BeginPlay() override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Setup();

	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<IMarkable> MarkableObj;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker")
	float ApproachDist = 100000;

	UFUNCTION(BlueprintCallable)
	FVector CalcApproachForces();

	UPROPERTY(BlueprintReadWrite, Category = "Marker")
	FVector ApproachForces;
	
	UPROPERTY(BlueprintReadWrite, Category = "Marker")
	UMarker* MarkerObj;

	UPROPERTY(BlueprintReadOnly, Category = "Marker")
	UMarkerWidget* MarkerWidget;

	UPROPERTY(BlueprintReadWrite, Category = "Marker")
	ELockType CurrentLockType;

public:
	UFUNCTION(BlueprintCallable)
	virtual void ToggleVisualLock(bool Active);

	UFUNCTION(BlueprintCallable)
	virtual void ToggleLock(UMarker* Marker);

	UFUNCTION(BlueprintCallable)
	FVector GetApproachForces() const { return ApproachForces; };

	UFUNCTION(BlueprintCallable)
	TScriptInterface<IMarkable> GetMarkedObj() const { return MarkableObj; }

};
