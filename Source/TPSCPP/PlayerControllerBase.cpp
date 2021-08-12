// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

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
