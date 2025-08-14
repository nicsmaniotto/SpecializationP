// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackHole.h"
#include "WhiteHole.h"

void ABlackHole::OnBeginOverlap_Implementation(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if (!LinkedWhiteHole || !OtherComp->IsSimulatingPhysics()) return;

	OtherActor->SetActorLocation(LinkedWhiteHole->GetRepositionLocation(), false, nullptr, ETeleportType::TeleportPhysics);
}
