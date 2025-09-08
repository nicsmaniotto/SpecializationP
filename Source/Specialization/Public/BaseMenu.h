// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Setuppable.h"
#include "BaseMenu.generated.h"

class UBaseHUD;
class UCanvasPanel;

/**
 * HUD base class.
 * It presents an instance of the BaseHUD widget in it.
 */
UCLASS()
class SPECIALIZATION_API UBaseMenu : public UUserWidget, public ISetuppable
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Base", meta = (BindWidget))
	UBaseHUD* BaseHUD;

public:
	/*Links all the events needed for functionality*/
	virtual void SetupEvents_Implementation(AActor* LinkedActor) override;
	
};
