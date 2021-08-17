// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Characters/CharacterBase.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TPSCPP_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnInteracted(ACharacterBase* Character) = 0;
};
