// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "Interactable.h"
#include "InteractableComponent.generated.h"

class ASpecializationCharacter;

/**
 * @see interface IInteractable
 * This capsule component detects the player and stores its reference in him.
 * Thus the player will interact with this component and this component will
 * call the interact function of the IInteractable implemented objects he found linked to itself (owner or brother components).
 */
UCLASS(ClassGroup = "Collision", editinlinenew, hidecategories = (Object, LOD, Lighting, TextureStreaming), meta = (DisplayName = "Interactable Component", BlueprintSpawnableComponent))
class SPECIALIZATION_API UInteractableComponent : public UCapsuleComponent, public IInteractable
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

	/*Fill InteractObjects with IInteractable implemented objects linked to this component*/
	void FillInteractComponents();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(BlueprintReadOnly)
	ASpecializationCharacter* Player;

	UPROPERTY()
	TArray<UObject*> InteractObjects;

public:
	void Interact_Implementation(class ASpecializationCharacter* _Player);
	
};
