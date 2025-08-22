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
struct FInputActionValue;

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

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* SpaceshipMappingContext;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UEngineAudioComponent* LeftEngine;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UEngineAudioComponent* RightEngine;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UEngineAudioComponent* TopRightEngine;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UEngineAudioComponent* TopLeftEngine;

public:
	// Sets default values for this pawn's properties
	ASpaceship();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	APawn* Possesser;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Possess_Implementation(APawn* _Possesser);

	UFUNCTION()
	void UnPossess_Implementation();

	UFUNCTION()
	AActor* GetPossesser_Implementation() { return Possesser; }
	
	void Interact_Implementation(ASpecializationCharacter* Player) override;

	UFUNCTION()
	void OnPossessMidTransition(ASpecializationCharacter* Player);
	
	UFUNCTION()
	void OnUnPossessMidTransition();

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

	/*Automatic pilot*/

	/** Called for mark/automatic pilot input */
	void LockObject(const FInputActionValue& Value);
	void AutomaticPilot(const FInputActionValue& Value);

	UFUNCTION()
	void OnGravityUpdate(FVector OldGForce, FVector NewGForce);
};
