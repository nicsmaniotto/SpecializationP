// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseMenu.generated.h"

class UBaseHUD;

/**
 * 
 */
UCLASS()
class SPECIALIZATION_API UBaseMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	UBaseHUD* BaseHUD;

public:
	void SetupEvents(AActor* LinkedActor);
	
};
