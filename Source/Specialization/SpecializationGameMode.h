// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpecializationGameMode.generated.h"

UCLASS(minimalapi)
class ASpecializationGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASpecializationGameMode();

	void BeginPlay() override;

	/* 
	* @See GravityBound
	* Gravity multiplier used by all gravity bounds for gravity execution
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gravity")
	float GravityMultiplier = 30000;

	static inline float GRAVITYMULTIPLIER;
};



