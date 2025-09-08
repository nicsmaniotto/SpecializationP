// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableComponent.h"
#include "Specialization/SpecializationCharacter.h"
#include <MyHUD.h>

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	FillInteractComponents();

	OnComponentBeginOverlap.AddUniqueDynamic(this, &UInteractableComponent::OnBeginOverlap);
	OnComponentEndOverlap.AddUniqueDynamic(this, &UInteractableComponent::OnEndOverlap);
}

void UInteractableComponent::FillInteractComponents()
{
	AActor* Owner = GetOwner();

	if (Owner->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		InteractObjects.Add(Owner);
	}

	InteractObjects.Append(Owner->GetComponentsByInterface(UInteractable::StaticClass()));

	InteractObjects.Remove(this);

	if (InteractObjects.Num() == 0) DestroyComponent();
}

void UInteractableComponent::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASpecializationCharacter* P = Cast<ASpecializationCharacter>(OtherActor);

	if (!P) return;

	Player = P;
	Player->CurrentInteractable = this;

	AMyHUD* HUD = Cast<AMyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	HUD->AskToggleWidget(true, EWidgetType::INTERACT);
}

void UInteractableComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ASpecializationCharacter* P = Cast<ASpecializationCharacter>(OtherActor);

	if (!P) return;

	if (Player->CurrentInteractable == this) Player->CurrentInteractable = nullptr;
	
	Player = nullptr;

	if (!GetWorld() || !GetWorld()->GetFirstPlayerController()) return;

	AMyHUD* HUD = Cast<AMyHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	HUD->AskToggleWidget(false, EWidgetType::INTERACT);
}

void UInteractableComponent::Interact_Implementation(ASpecializationCharacter* _Player)
{
	if(!Player) return;

	for (UObject* O : InteractObjects)
	{
		IInteractable::Execute_Interact(O, Player);
	}
}


