// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpecializationCharacter.h"
#include "SpecializationProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Spaceship.h"
#include "Kismet/GameplayStatics.h"
#include <Kismet/KismetMathLibrary.h>
#include <Jetpack.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <EnergyComponent.h>
#include <Planet.h>
#include <Marker.h>
#include "PhysicsEngine/PhysicsConstraintComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASpecializationCharacter

ASpecializationCharacter::ASpecializationCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	CameraSocket = CreateDefaultSubobject<USceneComponent>(TEXT("CameraSocket"));
	CameraSocket->SetupAttachment(GetCapsuleComponent());

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(CameraSocket);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	/*ConstraintMesh = CreateDefaultSubobject<UStaticMeshComponent>("Constraint Mesh");
	ConstraintMesh->SetupAttachment(GetCapsuleComponent());*/

	// Create fire engine
	Jetpack = CreateDefaultSubobject<UJetpack>(TEXT("Jetpack"));

	// Create energy component
	EnergyComponent = CreateDefaultSubobject<UEnergyComponent>(TEXT("Energy Component"));

	// Create Marker component
	MarkerComponent = CreateDefaultSubobject<UMarker>(TEXT("Marker Component"));
}

void ASpecializationCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	SetSpaceship();

	Jetpack->SetDependencyComponent(MarkerComponent, EnergyComponent);

	Jetpack->OnGravityUpdate.AddUniqueDynamic(this, &ASpecializationCharacter::OnGravityUpdate);

	Jetpack->OnEndAutomaticPilot.BindDynamic(this, &ASpecializationCharacter::OnEndAutomaticPilot);
}

void ASpecializationCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	EnergyComponent->StartConsumeEnergy(OxygenConsumptionMap);

	if (!GetController()) return;

	// camera pitch movement
	FRotator r = FirstPersonCameraComponent->GetRelativeRotation();
	r.Pitch = GetController()->GetControlRotation().Pitch;

	FirstPersonCameraComponent->SetRelativeRotation(r);

	GetCharacterMovement()->SetActive(false);

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetCapsuleComponent()->GetPhysicsLinearVelocity(), FColor::Blue, false, .1f);

	// adjust velocity with planet if present
	APlanet* PlanetSurface = Jetpack->GetPlanetSurface();
	//if (PlanetSurface)
	if (PlanetSurface && Jetpack->GetOnAir())
	{
		GetCapsuleComponent()->AddForce(
			(PlanetSurface->GetDeltaVelocity() + PlanetSurface->GetDeltaAngForce(GetCapsuleComponent()->GetComponentLocation())) * GetCapsuleComponent()->GetLinearDamping(),
			NAME_None, true);
	}

	if (bHasJumped && Jetpack->GetOnAir())
	{
		if (Jetpack->GetGForce().SquaredLength() > 0
			&& FVector::DotProduct(Jetpack->GetGForce(), GetCapsuleComponent()->GetPhysicsLinearVelocity()) > 0)
		{
			if (JumpTopFreedomTimer < JumpTopFreedomTime)
			{
				JumpTopFreedomTimer += DeltaSeconds;
				GetCapsuleComponent()->AddForce(-Jetpack->GetGForce() / JumpTopGravityDivider, NAME_None, true);
			}
			else
			{
				bHasJumped = false;
				JumpTopFreedomTimer = 0;
			}
		}
	}
}

void ASpecializationCharacter::OnGravityUpdate(FVector OldGForce, FVector NewGForce)
{
	MarkerComponent->ToggleSelf(NewGForce.SquaredLength() == 0);
}

void ASpecializationCharacter::OnEndAutomaticPilot()
{
	if (!MarkerComponent->ToggleTrajectory())
	{
		Jetpack->ToggleAutomaticPilot(false);
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void ASpecializationCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASpecializationCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Ongoing, this, &ASpecializationCharacter::JumpOnGoing);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ASpecializationCharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Started, this, &ASpecializationCharacter::StartMove);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpecializationCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASpecializationCharacter::StopMove);

		// Jetpack
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Started, this, &ASpecializationCharacter::StartMove);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASpecializationCharacter::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASpecializationCharacter::EndThrottle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Canceled, this, &ASpecializationCharacter::EndThrottle);

		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Triggered, this, &ASpecializationCharacter::Reverse);
		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Completed, this, &ASpecializationCharacter::EndReverse);
		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Canceled, this, &ASpecializationCharacter::EndReverse);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpecializationCharacter::Look);

		// Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ASpecializationCharacter::StartInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ASpecializationCharacter::StopInteract);

		// Marker
		EnhancedInputComponent->BindAction(MarkerAction, ETriggerEvent::Started, this, &ASpecializationCharacter::LockObject);

		// Automatic pilot
		EnhancedInputComponent->BindAction(AutomaticPilotAction, ETriggerEvent::Started, this, &ASpecializationCharacter::AutomaticPilot);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASpecializationCharacter::TogglePhysicality(bool Active)
{
	SetActorHiddenInGame(!Active);
	GetCapsuleComponent()->SetSimulatePhysics(Active);
	SetActorEnableCollision(Active);
}


void ASpecializationCharacter::StartMove(const FInputActionValue& Value)
{
	APlanet* PlanetSurface = Jetpack->GetPlanetSurface();
	//if (PlanetSurface)
	if (PlanetSurface && !Jetpack->GetOnAir())
	{
		FVector DampCompensation = GetCapsuleComponent()->GetLinearDamping() * GetCapsuleComponent()->GetPhysicsLinearVelocity();
		FVector PlanetForces = PlanetSurface->GetDeltaVelocity() + PlanetSurface->GetDeltaAngForce(GetCapsuleComponent()->GetComponentLocation());

		GetCapsuleComponent()->SetPhysicsLinearVelocity(PlanetForces);
	}

}

void ASpecializationCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	MovementVector.Normalize();

	if (bOnJetpack && Jetpack->GetOnAir())
	{
		Jetpack->Move(Value);
		return;
	}

	if (Controller != nullptr && !Jetpack->GetOnAir() && !bHasJumped)
	{
		// add movement 
		FVector PlanetVelocity = FVector::ZeroVector;

		APlanet* PlanetSurface = Jetpack->GetPlanetSurface();
		if (PlanetSurface)
		{
			PlanetVelocity = PlanetSurface->GetDeltaVelocity() + PlanetSurface->GetDeltaAngForce(GetCapsuleComponent()->GetComponentLocation());
		}

		FVector ForVector = GetActorForwardVector();

		FVector RightVector = GetActorRightVector();

		if (Jetpack->GetGForce().SquaredLength() > 0)
		{
			ForVector = FVector::VectorPlaneProject(ForVector, -Jetpack->GetGForce().GetSafeNormal()).GetSafeNormal();
			RightVector = FVector::CrossProduct(-Jetpack->GetGForce().GetSafeNormal(), ForVector);
		}

		LastMovDirection = (ForVector * MovementVector.Y + RightVector * MovementVector.X).GetSafeNormal();

		ForVector = PlanetVelocity + LastMovDirection * GetCharacterMovement()->MaxWalkSpeed;

		GetCapsuleComponent()->SetPhysicsLinearVelocity(ForVector, false);
	}
}

void ASpecializationCharacter::StopMove(const FInputActionValue& Value)
{
	if (bOnJetpack) Jetpack->StopMove(Value);

	LastMovDirection = FVector::ZeroVector;
}

void ASpecializationCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerPitchInput(LookAxisVector.Y);
	}

	GetCapsuleComponent()->AddTorqueInDegrees(GetCapsuleComponent()->GetUpVector() * LookAxisVector.X * FMath::Square(TorqueForce), NAME_None, true);
}

void ASpecializationCharacter::StartInteract(const FInputActionValue& Value)
{
	if (!CurrentInteractable || !IsValid(CurrentInteractable->_getUObject())) return;

	IInteractable::Execute_Interact(CurrentInteractable->_getUObject(), this);
}

void ASpecializationCharacter::StopInteract(const FInputActionValue& Value)
{
}

void ASpecializationCharacter::LockObject(const FInputActionValue& Value)
{
	if (!Jetpack->IsInAtmosphere()) MarkerComponent->ToggleMarkObject();
}

void ASpecializationCharacter::AutomaticPilot(const FInputActionValue& Value)
{
	if (Jetpack->IsInAtmosphere()) return;

	if (Jetpack->GetIsAutomaticPilot())
	{
		Jetpack->ToggleAutomaticPilot(false);
	}
	else
	{
		if (MarkerComponent->ToggleTrajectory())
		{
			Jetpack->ToggleAutomaticPilot(true);
		}
	}

}

void ASpecializationCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ASpecializationCharacter::GetHasRifle()
{
	return bHasRifle;
}

void ASpecializationCharacter::Possess_Implementation(APawn* Possesser)
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Possesser->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			PlayerController->Possess(this);
		}
	}

	TogglePhysicality(true);

	OnSpaceshipInteraction.Broadcast(false);
}

void ASpecializationCharacter::UnPossess_Implementation()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(DefaultMappingContext);
		}
	}

	TogglePhysicality(false);

	OnSpaceshipInteraction.Broadcast(true);
}

void ASpecializationCharacter::Jump()
{
	if (!JumpForceCurve || Jetpack->GetOnAir()) return;

	Crouch();

	JumpHoldTimer = 0;
}

void ASpecializationCharacter::JumpOnGoing()
{
	if (!JumpForceCurve) return;

	JumpHoldTimer += GetWorld()->GetDeltaSeconds();

	GEngine->AddOnScreenDebugMessage(-1, .1, FColor::Red, FString::Printf(TEXT("Jump timer: %f"), JumpHoldTimer));
}

void ASpecializationCharacter::StopJumping()
{
	if (!JumpForceCurve) return;

	if (JumpHoldTimer > 0)
	{
		UnCrouch();
	}

	if (Jetpack->GetOnAir()) return;

	FVector JumpDir = GetCapsuleComponent()->GetUpVector();
	if (Jetpack->GetGForce().SquaredLength() > 0)
	{
		JumpDir += LastMovDirection * JumpDirectionMultiplier;
	}

	float ForceToApply = JumpForceCurve->GetFloatValue(JumpHoldTimer);

	GetCapsuleComponent()->AddImpulse(JumpDir.GetSafeNormal() * ForceToApply, NAME_None, true);

	bHasJumped = true;
}

void ASpecializationCharacter::Crouch(bool bClientSimulation)
{
	ToggleCrouch(true);
}

void ASpecializationCharacter::UnCrouch(bool bClientSimulation)
{
	ToggleCrouch(false);
}

void ASpecializationCharacter::SetSpaceship()
{
	if (Spaceship) return;

	Spaceship = Cast<ASpaceship>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpaceship::StaticClass()));
}

void ASpecializationCharacter::Throttle(const FInputActionValue& Value)
{
	if (bOnJetpack)
	{
		Jetpack->Throttle(Value);
	}
}

void ASpecializationCharacter::EndThrottle(const FInputActionValue& Value)
{
	Jetpack->EndThrottle(Value);
}

void ASpecializationCharacter::Reverse(const FInputActionValue& Value)
{
	if (bOnJetpack && Jetpack->GetOnAir())
	{
		Jetpack->Reverse(Value);
	}
}

void ASpecializationCharacter::EndReverse(const FInputActionValue& Value)
{
	if (bOnJetpack && Jetpack->GetOnAir())
	{
		Jetpack->StopReverse(Value);
	}
}
