// Fill out your copyright notice in the Description page of Project Settings.


#include "Planet.h"
#include "GravityBound.h"
#include "MarkingComponent.h"
#include "Components/SplineComponent.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values
APlanet::APlanet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);

	GravityBound = CreateDefaultSubobject<UGravityBound>("Gravity Bound");
	GravityBound->SetupAttachment(Root);

	Orbit = CreateDefaultSubobject<USplineComponent>("Orbit");
	Orbit->SetupAttachment(Root);

	MarkingComponent = CreateDefaultSubobject<UMarkingComponent>("Marking Component");
	MarkingComponent->SetupAttachment(GravityBound);
}

USceneComponent* APlanet::GetMarkedObject_Implementation()
{
	return Mesh;
}

// Called when the game starts or when spawned
void APlanet::BeginPlay()
{
	Super::BeginPlay();

	// Trick to enable collision on already overlapping objects
	SetActorEnableCollision(false);

	FTimerHandle H;
	FTimerDelegate D;
	D.BindLambda([&]()->void {SetActorEnableCollision(true); });

	GetWorld()->GetTimerManager().SetTimer(H, D, .1f, false);
	// end of the trick

	SetupChildren();
}

void APlanet::SetupChildren()
{
	TArray<AActor*> Out;
	GetAttachedActors(Out, true, true);

	FAttachmentTransformRules Rules = FAttachmentTransformRules(
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		EAttachmentRule::KeepWorld,
		true);

	for (AActor* A : Out)
	{
		A->AttachToComponent(Mesh, Rules);
	}

	GravityBound->AttachToComponent(Mesh, Rules);
}


// Called every frame
void APlanet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MeshRotation(DeltaTime);

	OrbitFollow(DeltaTime);
}

FVector APlanet::GetDeltaVelocity() const
{
	return Mesh->GetPhysicsLinearVelocity();
}

FVector APlanet::GetDeltaAngForce(FVector Location) const
{
	FVector AngVelForce = FVector::CrossProduct(Mesh->GetPhysicsAngularVelocityInRadians(), Location - Mesh->GetComponentLocation());

	DrawDebugLine(GetWorld(), GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation(),
		GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation() - AngVelForce, FColor::Emerald, false, .1f);

	return AngVelForce;
}

void APlanet::MeshRotation(float DeltaTime)
{
	FRotator r = FRotator(0, RotationSpeed * DeltaTime, 0);

	Mesh->AddLocalRotation(r.Quaternion(), true);
}

void APlanet::OrbitFollow(float DeltaTime)
{
	PositionInSpline += OrbitSpeed * DeltaTime;

	RepositionMesh();
}

void APlanet::RepositionMesh()
{
	if (PositionInSpline >= Orbit->GetSplineLength())
	{
		PositionInSpline -= (int)PositionInSpline;
	}

	FVector NextPos = Orbit->GetLocationAtDistanceAlongSpline(PositionInSpline, ESplineCoordinateSpace::World);
	Mesh->SetWorldLocation(NextPos, true);
}