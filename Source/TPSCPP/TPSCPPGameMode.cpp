// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "TPSCPPGameMode.h"
#include "TPSCPPHUD.h"
#include "TPSCPPCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATPSCPPGameMode::ATPSCPPGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/MyAssets/Characters/Blueprint/BPPCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ATPSCPPHUD::StaticClass();
}
