// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Interactable.h"
#include "InteractableComponent.generated.h"

class ASpecializationCharacter;

/**
 * 
 */
UCLASS(ClassGroup = "Collision", editinlinenew, hidecategories = (Object, LOD, Lighting, TextureStreaming), meta = (DisplayName = "Interactable Component", BlueprintSpawnableComponent))
class SPECIALIZATION_API UInteractableComponent : public UCapsuleComponent, public IInteractable
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

	void FillInteractComponents();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	ASpecializationCharacter* Player;

	UPROPERTY()
	TArray<TScriptInterface<IInteractable>> InteractObjects;

public:
	void Interact_Implementation(class ASpecializationCharacter* _Player);
	
};
