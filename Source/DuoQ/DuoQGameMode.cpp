// Copyright Epic Games, Inc. All Rights Reserved.

#include "DuoQGameMode.h"
#include "DuoQCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADuoQGameMode::ADuoQGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	// DefaultPawnClass = PlayerPawnClassFinder.Class;

}
