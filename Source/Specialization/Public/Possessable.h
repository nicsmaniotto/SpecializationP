// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Possessable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UPossessable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPECIALIZATION_API IPossessable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Possess(APawn* Possesser);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UnPossess();
};
