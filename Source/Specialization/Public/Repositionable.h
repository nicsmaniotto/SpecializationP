// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Repositionable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URepositionable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPECIALIZATION_API IRepositionable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	USceneComponent* GetRepositionableComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AskReposition(ERepositionType RepositionType, FVector RepositionTorqueForce, bool ForceReposition = false);
};
