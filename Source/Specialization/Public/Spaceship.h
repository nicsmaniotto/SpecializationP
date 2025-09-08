// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Possessable.h"
#include "Interactable.h"
#include "Spaceship.generated.h"

class APlayerController;
class ASpecializationCharacter;
class UInputMappingContext;
class UEngineAudioComponent;
class USpaceshipWidgetComponent;
struct FInputActionValue;


/**
* Spaceship class.
* Can be possessed to drive the spaceship.
* It redirects its input to the fire engine to fly.
*/
UCLASS()
class SPECIALIZATION_API ASpaceship : public APawn, public IPossessable, public IInteractable
{
	GENERATED_BODY()

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ThrottleAction;
	
	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReverseAction;

	/** Always active mapping context  when possessed*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* SpaceshipMappingContext;
	
	/** Active mapping context  when possessed and not on automatic pilot*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* SpaceshipManualMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CollisionComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UFireEngine* FireEngine;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UInteractableComponent* InteractComponent;

	/** Marker Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MarkerAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UMarker* MarkerComponent;

	/** Marker Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AutomaticPilotAction;
	
	/** Toggle Jetpack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleJetpackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UEngineAudioComponent* LeftEngine;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UEngineAudioComponent* RightEngine;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UEngineAudioComponent* TopRightEngine;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UEngineAudioComponent* TopLeftEngine;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	USpaceshipWidgetComponent* SpaceshipWidgetComponent;

public:
	// Sets default values for this pawn's properties
	ASpaceship();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Possession
public:
	UFUNCTION()
	void Possess_Implementation(APawn* _Possesser);

	UFUNCTION()
	void UnPossess_Implementation();
	
	UFUNCTION()
	void ToggleContext(APlayerController* PlayerController, UInputMappingContext* MappingContext, bool IsAdding) const;

	UFUNCTION()
	AActor* GetPossesser_Implementation() { return Possesser; }
	
	void Interact_Implementation(ASpecializationCharacter* Player) override;

protected:
	UFUNCTION()
	void OnPossessMidTransition(ASpecializationCharacter* Player);
	
	UFUNCTION()
	void OnUnPossessMidTransition();
	
	/*Should be player!!*/
	UPROPERTY(BlueprintReadOnly, Category = "Possesser")
	APawn* Possesser;

	// Movement
protected:
	void Move(const FInputActionValue& Value);
	void StopMove(const FInputActionValue& Value);

	void Throttle(const FInputActionValue& Value);
	void EndThrottle(const FInputActionValue& Value);
	
	void Reverse(const FInputActionValue& Value);
	void EndReverse(const FInputActionValue& Value);

	void StartLook(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StopLook(const FInputActionValue& Value);

	/** Called for looking input */
	void StartInteract(const FInputActionValue& Value);
	void StopInteract(const FInputActionValue& Value);

	//Automatic pilot
protected:
	/** Called for mark/automatic pilot input */
	void LockObject(const FInputActionValue& Value);

	/*Toggle pilot logic*/
	void AutomaticPilot(const FInputActionValue& Value);

	UFUNCTION()
	void OnGravityUpdate(FVector OldGForce, FVector NewGForce);
	
	/*Remove manual mapping context if on automatic pilot*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	void OnAutomaticPilot(bool Active);

	// Suit Interaction
protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void JetpackInteraction();
};
