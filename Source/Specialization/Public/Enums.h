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
