// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Possessable.h"
#include "Interactable.h"
#include "Enums.h"
#include "Repositionable.h"
#include "SpecializationCharacter.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpaceshipInteraction, bool, OnSpaceship);

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class ASpaceship;
class UEnergyComponent;
class UMarker;
class UJetpack;
struct FInputActionValue;
struct FBodyInstance;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);


/**
* 
*/
UCLASS(config = Game)
class ASpecializationCharacter : public ACharacter, public IPossessable, public IRepositionable
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ConstraintMesh;*/

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USceneComponent* CameraSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* JetpackMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Throttle Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrottleAction;

	/** Reverse Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReverseAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	/** Marker Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MarkerAction;

	/** Marker Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AutomaticPilotAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UMarker* MarkerComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UJetpack* Jetpack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Energy, meta = (AllowPrivateAccess = "true"))
	UEnergyComponent* EnergyComponent;

public:
	ASpecializationCharacter();

protected:
	virtual void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/*Called on possess/unpossess*/
	void TogglePhysicality(bool Active);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	void Possess(ASpaceship* _Spaceship);

	UFUNCTION()
	void Possess_Implementation(APawn* Possesser);

	void UnPossess();

	UFUNCTION()
	void UnPossess_Implementation();

	AActor* GetPossesser();

	UFUNCTION()
	AActor* GetPossesser_Implementation() { return nullptr; }

	// Movement
protected:
	/** Called for movement input */
	void StartMove(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void StopMove(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for looking input */
	void StartInteract(const FInputActionValue& Value);
	void StopInteract(const FInputActionValue& Value);

	/** Called for mark/automatic pilot input */
	void LockObject(const FInputActionValue& Value);
	void AutomaticPilot(const FInputActionValue& Value);

	// Jump
	void Jump() override;
	void JumpOnGoing();
	void StopJumping() override;

	void Crouch(bool bClientSimulation = false) override;
	void UnCrouch(bool bClientSimulation = false) override;

	UFUNCTION(BlueprintImplementableEvent)
	void ToggleCrouch(bool Active);

	// Look
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Look, meta = (AllowPrivateAccess = "true"))
	float TorqueForce = 100;

	// Spaceship
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	ASpaceship* Spaceship;

public:
	UFUNCTION(BlueprintCallable)
	ASpaceship* GetSpaceship() const { return Spaceship; }

	UFUNCTION(BlueprintCallable)
	void SetSpaceship();

	UPROPERTY(BlueprintAssignable)
	FOnSpaceshipInteraction OnSpaceshipInteraction;

	// Jetpack
protected:
	void Throttle(const FInputActionValue& Value);
	void EndThrottle(const FInputActionValue& Value);

	void Reverse(const FInputActionValue& Value);
	void EndReverse(const FInputActionValue& Value);

	UFUNCTION()
	void OnGravityUpdate(FVector OldGForce, FVector NewGForce);
	
	UFUNCTION()
	void OnJetpackEquip(bool IsEquipped);

	UFUNCTION(BlueprintCallable)
	bool ToggleJetpack();

	// cheap change of materials for jetpack equip/unequip
	/*unequipped jetpack materials*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jetpack")
	TArray<UMaterial*> NormalMaterials;
	
	/*equipped jetpack materials*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jetpack")
	TArray<UMaterial*> JetpackMaterials;


public:
	UFUNCTION(BlueprintCallable)
	UJetpack* GetJetpack() const { return Jetpack; }

	// Interaction
public:
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TScriptInterface<IInteractable> CurrentInteractable;

	// Energy
public:
	UFUNCTION(BlueprintCallable)
	UEnergyComponent* GetEnergyComponent() const { return EnergyComponent; }

	/*
	* @See class EnergyComponent
	* Map for oxygen consumption
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Energy")
	TMap<EEnergyType, bool> OxygenConsumptionMap;

	// Reposition
public:
	USceneComponent* GetRepositionableComponent_Implementation() const override { return CameraSocket; };
};

