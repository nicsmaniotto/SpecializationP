// Fill out your copyright notice in the Description page of Project Settings.


#include "WhiteHole.h"
#include "Components/SphereComponent.h"

void AWhiteHole::BeginPlay()
{
	Super::BeginPlay();

	HoleCollision->OnComponentEndOverlap.AddUniqueDynamic(this, &AWhiteHole::OnEndOverlap);
}

void AWhiteHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (UPrimitiveComponent* P : PComponents)
	{
		FVector Vel = P->GetComponentLocation() - GetActorLocation();
		Vel.Normalize();

		P->AddImpulse(Vel * RepulsionForce, NAME_None, true);
	}
}

void AWhiteHole::OnBeginOverlap_Implementation(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherComp->IsSimulatingPhysics()) return;

	if (PComponents.Contains(OtherComp)) return;

	FVector Vel = OtherComp->GetComponentLocation() - GetActorLocation();
	Vel.Normalize();

	OtherComp->SetAllPhysicsLinearVelocity(FVector::ZeroVector);

	PComponents.Add(OtherComp);

	//OtherComp->AddImpulse(Vel * RepulsionForce, NAME_None, true);
}

void AWhiteHole::OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!OtherComp->IsSimulatingPhysics()) return;

	if (!PComponents.Contains(OtherComp)) return;

	PComponents.Remove(OtherComp);
}

