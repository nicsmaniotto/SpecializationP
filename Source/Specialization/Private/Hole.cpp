// Fill out your copyright notice in the Description page of Project Settings.


#include "Hole.h"
#include "Components/SphereComponent.h"


// Sets default values
AHole::AHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);
	
	HoleCollision = CreateDefaultSubobject<USphereComponent>("HoleCollision");
	HoleCollision->SetupAttachment(Root);

}

// Called when the game starts or when spawned
void AHole::BeginPlay()
{
	Super::BeginPlay();

	HoleCollision->OnComponentBeginOverlap.AddUniqueDynamic(this, &AHole::OnBeginOverlap);
}

// Called every frame
void AHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

