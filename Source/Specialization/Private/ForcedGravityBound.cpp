// Fill out your copyright notice in the Description page of Project Settings.


#include "ForcedGravityBound.h"
#include <Kismet/KismetMathLibrary.h>

void UForcedGravityBound::EnlistComponent(UPrimitiveComponent* OtherComp)
{
	for (AActor* O : Outers)
	{
		UGravityBound* b = O->GetComponentByClass<UGravityBound>();
		if (!b) continue;

		b->UnenlistComponent(OtherComp);
	}

	if (OtherComp->IsSimulatingPhysics())
	{
		OtherComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

		FVector LinearVelocity = OtherComp->GetPhysicsLinearVelocity();
		LinearVelocity.Normalize();

		FVector COG = UKismetMathLibrary::TransformLocation(GetComponentTransform(), CenterOfGravity);
		FVector ReversedCOG = UKismetMathLibrary::TransformLocation(GetComponentTransform(), -CenterOfGravity);
		//FVector Dir = COG - OtherComp->GetComponentLocation();
		FVector Dir = FMath::ClosestPointOnLine(ReversedCOG, COG, OtherComp->GetComponentLocation()) - OtherComp->GetComponentLocation();

		LinearVelocity = Dir.GetSafeNormal() * LinearVelocityAdjusted;

		OtherComp->SetPhysicsLinearVelocity(LinearVelocity);
	}

	Super::EnlistComponent(OtherComp);
}

void UForcedGravityBound::UnenlistComponent(UPrimitiveComponent* OtherComp)
{
	Super::UnenlistComponent(OtherComp);

	for (AActor* O : Outers)
	{
		UGravityBound* b = O->GetComponentByClass<UGravityBound>();
		if (!b) continue;

		b->EnlistComponent(OtherComp);
	}
}
