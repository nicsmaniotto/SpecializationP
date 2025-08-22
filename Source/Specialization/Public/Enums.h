// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Enums.generated.h"

UENUM(BlueprintType)
enum class EWidgetType : uint8 {
    BASIC = 0 UMETA(DisplayName = "BASIC"),
    SHIP = 1  UMETA(DisplayName = "SHIP"),
    INTERACT = 2    UMETA(DisplayName = "INTERACT"),
    TRANSITION = 3    UMETA(DisplayName = "TRANSITION"),
    CUSTOM = 4  UMETA(DisplayName = "CUSTOM"),
};

UENUM(BlueprintType)
enum class ERepositionType : uint8 {
    FORWARD = 0 UMETA(DisplayName = "FORWARD"),
    RIGHT = 1  UMETA(DisplayName = "RIGHT"),
};

UENUM(BlueprintType)
enum class EEnergyType : uint8 {
    NONE = 0 UMETA(DisplayName = "NONE"),
    OXYGEN = 1 UMETA(DisplayName = "OXYGEN"),
    FUEL = 2  UMETA(DisplayName = "FUEL"),
};

USTRUCT(BlueprintType, meta = (EditInlineNew, Blueprintable))
struct FEnergyInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EEnergyType EnergyType = EEnergyType::NONE;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float GrowthValue = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HardGrowthValue = .8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DecayValue = .2f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HardDecayValue = .8f;
};

UENUM(BlueprintType)
enum class EIndicatorType : uint8 {
    LEFT = 0 UMETA(DisplayName = "LEFT"),
    TOP = 1 UMETA(DisplayName = "TOP"),
    RIGHT = 2  UMETA(DisplayName = "RIGHT"),
    BOTTOM = 3  UMETA(DisplayName = "BOTTOM"),
};

UENUM(BlueprintType)
enum class EEnginePosition : uint8 {
    CENTER = 0 UMETA(DisplayName = "CENTER"),
    TOP = 1 UMETA(DisplayName = "TOP"),
    RIGHT = 2  UMETA(DisplayName = "RIGHT"),
    LEFT = 3  UMETA(DisplayName = "LEFT"),
};

UENUM(BlueprintType)
enum class EBarPosition : uint8 {
    FRONT = 0 UMETA(DisplayName = "FRONT"),
    BACK = 1 UMETA(DisplayName = "BACK"),
    TOP = 2 UMETA(DisplayName = "TOP"),
    BOTTOM = 3 UMETA(DisplayName = "BOTTOM"),
    RIGHT = 4  UMETA(DisplayName = "RIGHT"),
    LEFT = 5  UMETA(DisplayName = "LEFT"),
};

UENUM(BlueprintType)
enum class ELockType : uint8 {
    NONE = 0 UMETA(DisplayName = "NONE"),
    LOCKED = 1 UMETA(DisplayName = "LOCKED"),
};
