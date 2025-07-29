// Fill out your copyright notice in the Description page of Project Settings.


#include "WhiteHole.h"

void AWhiteHole::OnBeginOverlap_Implementation(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherComp->IsSimulatingPhysics()) return;

	FVector Vel = OtherComp->GetPhysicsLinearVelocity();
	Vel.Normalize();

	OtherComp->SetAllPhysicsLinearVelocity(FVector::ZeroVector);

	OtherComp->AddImpulse(-Vel * RepulsionForce, NAME_None, true);
}

