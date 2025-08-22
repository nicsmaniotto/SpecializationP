// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Setuppable.h"
#include "BaseMenu.generated.h"

class UBaseHUD;
class UCanvasPanel;

/**
 * 
 */
UCLASS()
class SPECIALIZATION_API UBaseMenu : public UUserWidget, public ISetuppable
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	UBaseHUD* BaseHUD;

public:
	virtual void SetupEvents_Implementation(AActor* LinkedActor) override;
	
};
