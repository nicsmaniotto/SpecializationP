// Fill out your copyright notice in the Description page of Project Settings.


#include "Spaceship.h"
#include <EnhancedInputComponent.h>
#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include <Specialization/SpecializationCharacter.h>
#include <Kismet/KismetMathLibrary.h>
#include <FireEngine.h>

// Sets default values
ASpaceship::ASpaceship()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->bCastDynamicShadow = false;
	Mesh->CastShadow = false;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(Mesh);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create fire engine
	FireEngine = CreateDefaultSubobject<UFireEngine>(TEXT("Fire Engine"));

}

// Called when the game starts or when spawned
void ASpaceship::BeginPlay()
{
	Super::BeginPlay();

	ASpecializationCharacter* Player = Cast<ASpecializationCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	Player->SetSpaceship(this);

}

// Called every frame
void ASpaceship::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASpaceship::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ASpaceship::StartInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ASpaceship::StopInteract);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpaceship::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASpaceship::StopMove);

		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Ongoing, this, &ASpaceship::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASpaceship::EndThrottle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Canceled, this, &ASpaceship::EndThrottle);
		
		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Ongoing, this, &ASpaceship::Reverse);
		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Completed, this, &ASpaceship::EndReverse);
		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Canceled, this, &ASpaceship::EndReverse);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Started, this, &ASpaceship::StartLook);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpaceship::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Completed, this, &ASpaceship::StopLook);
	}

}

void ASpaceship::Possess_Implementation(APawn* _Possesser)
{
	if (!Possesser) Possesser = _Possesser;

	if (APlayerController* PlayerController = Cast<APlayerController>(Possesser->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SpaceshipMappingContext, 0);
			PlayerController->Possess(this);
		}
	}
}

void ASpaceship::UnPossess_Implementation()
{
	if (!Possesser) return;

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(SpaceshipMappingContext);
		}
	}
}

void ASpaceship::Move(const FInputActionValue& Value)
{
	if (!FireEngine->GetOnAir()) return;

	if (Controller != nullptr) FireEngine->Move(Value);
}

void ASpaceship::StopMove(const FInputActionValue& Value)
{
	if (Controller != nullptr) FireEngine->StopMove(Value);
}

void ASpaceship::Throttle(const FInputActionValue& Value)
{
	FireEngine->Throttle(Value);
}

void ASpaceship::EndThrottle(const FInputActionValue& Value)
{
	FireEngine->EndThrottle(Value);
}

void ASpaceship::Reverse(const FInputActionValue& Value)
{
	FireEngine->Reverse(Value);
}

void ASpaceship::EndReverse(const FInputActionValue& Value)
{
	FireEngine->StopReverse(Value);
}

void ASpaceship::StartLook(const FInputActionValue& Value)
{
	FireEngine->StartLook(Value);
}

void ASpaceship::Look(const FInputActionValue& Value)
{
	if (!FireEngine->GetOnAir()) return;

	if (Controller != nullptr) FireEngine->Look(Value);
}

void ASpaceship::StopLook(const FInputActionValue& Value)
{
	FireEngine->StopLook(Value);
}

void ASpaceship::StartInteract(const FInputActionValue& Value)
{
	IPossessable::Execute_UnPossess(this);

	if (Possesser->GetClass()->ImplementsInterface(UPossessable::StaticClass()))
		IPossessable::Execute_Possess(Possesser, this);
}

void ASpaceship::StopInteract(const FInputActionValue& Value)
{
}

