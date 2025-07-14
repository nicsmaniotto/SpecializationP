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
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
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

	// Create fire engine
	Jetpack = CreateDefaultSubobject<UJetpack>(TEXT("Jetpack"));
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

}

void ASpecializationCharacter::Tick(float DeltaSeconds)
{
	if (!GetController()) return;

	// camera pitch movement
	FRotator r = FirstPersonCameraComponent->GetRelativeRotation();
	r.Pitch = GetController()->GetControlRotation().Pitch;

	FirstPersonCameraComponent->SetRelativeRotation(r);
}

//////////////////////////////////////////////////////////////////////////// Input

void ASpecializationCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpecializationCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASpecializationCharacter::StopMove);

		// Jetpack
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Ongoing, this, &ASpecializationCharacter::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASpecializationCharacter::EndThrottle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Canceled, this, &ASpecializationCharacter::EndThrottle);

		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Ongoing, this, &ASpecializationCharacter::Reverse);
		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Completed, this, &ASpecializationCharacter::EndReverse);
		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Canceled, this, &ASpecializationCharacter::EndReverse);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpecializationCharacter::Look);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ASpecializationCharacter::StartInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ASpecializationCharacter::StopInteract);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ASpecializationCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (bOnJetpack && Jetpack->GetOnAir())
	{
		Jetpack->Move(Value);
		return;
	}

	if (Controller != nullptr)
	{
		// add movement 
		GetCapsuleComponent()->AddImpulse(GetActorForwardVector() * MovementVector.Y * Speed, NAME_None, true);
		GetCapsuleComponent()->AddImpulse(GetActorRightVector() * MovementVector.X * Speed, NAME_None, true);
	}
}

void ASpecializationCharacter::StopMove(const FInputActionValue& Value)
{
	if (bOnJetpack) Jetpack->StopMove(Value);
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
	IPossessable::Execute_UnPossess(this);

	if (Spaceship->GetClass()->ImplementsInterface(UPossessable::StaticClass()))
		IPossessable::Execute_Possess(Spaceship, this);
}

void ASpecializationCharacter::StopInteract(const FInputActionValue& Value)
{
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
	Jetpack->StopReverse(Value);
}
