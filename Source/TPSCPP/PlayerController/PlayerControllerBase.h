// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	APlayerControllerBase();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
	class UUserWidget* HUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> HUDClass;
};
