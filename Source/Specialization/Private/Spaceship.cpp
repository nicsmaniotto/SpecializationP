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
#include <InteractableComponent.h>
#include <MyHUD.h>
#include <Marker.h>
#include <EngineAudioComponent.h>

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

	// CreateInteractionComponent
	InteractComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interact Component"));
	InteractComponent->SetupAttachment(Mesh);

	// Create Marker component
	MarkerComponent = CreateDefaultSubobject<UMarker>(TEXT("Marker Component"));

	// Create Engine Audio Components
	LeftEngine = CreateDefaultSubobject<UEngineAudioComponent>(TEXT("Left Engine Audio"));
	LeftEngine->SetupAttachment(Mesh);

	RightEngine = CreateDefaultSubobject<UEngineAudioComponent>(TEXT("Right Engine Audio"));
	RightEngine->SetupAttachment(Mesh);
	
	TopRightEngine = CreateDefaultSubobject<UEngineAudioComponent>(TEXT("TopRight Engine Audio"));
	TopRightEngine->SetupAttachment(Mesh);
	
	TopLeftEngine = CreateDefaultSubobject<UEngineAudioComponent>(TEXT("TopLeft Engine Audio"));
	TopLeftEngine->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void ASpaceship::BeginPlay()
{
	Super::BeginPlay();

	FireEngine->SetDependencyComponent(MarkerComponent);

	FireEngine->OnGravityUpdate.AddUniqueDynamic(this, &ASpaceship::OnGravityUpdate);
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

		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ASpaceship::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ASpaceship::EndThrottle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Canceled, this, &ASpaceship::EndThrottle);

		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Triggered, this, &ASpaceship::Reverse);
		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Completed, this, &ASpaceship::EndReverse);
		EnhancedInputComponent->BindAction(ReverseAction, ETriggerEvent::Canceled, this, &ASpaceship::EndReverse);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Started, this, &ASpaceship::StartLook);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpaceship::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Completed, this, &ASpaceship::StopLook);

		// Marker
		EnhancedInputComponent->BindAction(MarkerAction, ETriggerEvent::Started, this, &ASpaceship::LockObject);
		EnhancedInputComponent->BindAction(AutomaticPilotAction, ETriggerEvent::Triggered, this, &ASpaceship::AutomaticPilot);
	}

}

void ASpaceship::Possess_Implementation(APawn* _Possesser)
{
	Possesser = _Possesser;

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

	Possesser->SetActorLocationAndRotation(InteractComponent->GetComponentLocation(), InteractComponent->GetComponentRotation(), false, nullptr, ETeleportType::TeleportPhysics);
}

void ASpaceship::Interact_Implementation(ASpecializationCharacter* Player)
{
	FOnTransition T;
	T.BindLambda([&, this, Player]()->void { OnPossessMidTransition(Player); });

	FOnTransitionEnd TE;
	TE.BindLambda([&]()->void { EnableInput(GetWorld()->GetFirstPlayerController()); });

	AMyHUD* HUD = Cast<AMyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (HUD->ExecuteTransition(T, TE))
	{
		DisableInput(GetWorld()->GetFirstPlayerController());
	}
}

void ASpaceship::OnPossessMidTransition(ASpecializationCharacter* Player)
{
	IPossessable::Execute_UnPossess(Player);

	IPossessable::Execute_Possess(this, Player);
}

void ASpaceship::StartInteract(const FInputActionValue& Value)
{
	FOnTransition T;
	T.BindLambda([&, this]()->void { OnUnPossessMidTransition(); });

	FOnTransitionEnd TE;
	TE.BindLambda([&]()->void { EnableInput(GetWorld()->GetFirstPlayerController()); });

	AMyHUD* HUD = Cast<AMyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (HUD->ExecuteTransition(T, TE))
	{
		DisableInput(GetWorld()->GetFirstPlayerController());
	}
}

void ASpaceship::OnUnPossessMidTransition()
{
	IPossessable::Execute_UnPossess(this);

	if (Possesser->GetClass()->ImplementsInterface(UPossessable::StaticClass()))
	{
		IPossessable::Execute_Possess(Possesser, this);
	}

	Possesser = nullptr;
}

void ASpaceship::StopInteract(const FInputActionValue& Value)
{

}

void ASpaceship::LockObject(const FInputActionValue& Value)
{
	if (!FireEngine->IsInAtmosphere()) MarkerComponent->ToggleMarkObject();
}

void ASpaceship::AutomaticPilot(const FInputActionValue& Value)
{
	if (FireEngine->IsInAtmosphere()) return;

	if (FireEngine->GetIsAutomaticPilot())
	{
		FireEngine->ToggleAutomaticPilot(false);
	}
	else
	{
		FireEngine->ToggleAutomaticPilot(true);
	}
}

void ASpaceship::OnGravityUpdate(FVector OldGForce, FVector NewGForce)
{
	MarkerComponent->ToggleSelf(NewGForce.SquaredLength() == 0);
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