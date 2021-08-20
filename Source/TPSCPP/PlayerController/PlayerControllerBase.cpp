// Programmer: Khoi Ho


#include "PlayerControllerBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (HUDClass)
		{
			HUD = CreateWidget<UUserWidget>(this, HUDClass);
			HUD->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("HUDClass=nullptr"));
		}
	}
}

void APlayerControllerBase::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Respawn", EInputEvent::IE_Pressed, this, &APlayerControllerBase::InputRespawn);
}

APlayerControllerBase::APlayerControllerBase():Super()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> HUDClassToFind(TEXT("/Game/MyAssets/Characters/HUD/HUD_Main"));
	if (HUDClassToFind.Succeeded())
	{
		HUDClass = HUDClassToFind.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find the HUD class."));
	}
}

void APlayerControllerBase::InputRespawn()
{
	Respawn();
}

bool APlayerControllerBase::Respawn_Validate()
{
	return true;
}

void APlayerControllerBase::Respawn_Implementation()
{
	if (!IsValid(GetPawn()))
	{
		AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
		GameMode->RestartPlayer(this);
	}
}
