// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpecializationGameMode.h"
#include "SpecializationCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASpecializationGameMode::ASpecializationGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
