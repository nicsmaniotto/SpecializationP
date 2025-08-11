// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Possessable.h"
#include "Interactable.h"
#include "Enums.h"
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
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class ASpecializationCharacter : public ACharacter, public IPossessable
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh1P;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ConstraintMesh;*/

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UJetpack* Jetpack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Energy, meta = (AllowPrivateAccess = "true"))
	class UEnergyComponent* EnergyComponent;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;
	
	/** Marker Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MarkerAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UMarker* MarkerComponent;

	/** Marker Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AutomaticPilotAction;

public:
	ASpecializationCharacter();

protected:
	virtual void BeginPlay();

	void Tick(float DeltaSeconds) override;

public:

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

protected:
	/** Called for movement input */
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

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bOnJetpack = false;

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
	void OnEndAutomaticPilot();

	// Interaction
public:
	UPROPERTY(BlueprintReadWrite, Category = "Interaction")
	TScriptInterface<IInteractable> CurrentInteractable;

	// Energy
public:
	UFUNCTION(BlueprintCallable)
	UEnergyComponent* GetEnergyComponent() const { return EnergyComponent; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Energy")
	TMap<EEnergyType, bool> OxygenConsumptionMap;
};

