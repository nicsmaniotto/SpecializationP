// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Possessable.h"
#include "Spaceship.generated.h"

class APlayerController;
class ASpecializationCharacter;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class SPECIALIZATION_API ASpaceship : public APawn, public IPossessable
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
};
