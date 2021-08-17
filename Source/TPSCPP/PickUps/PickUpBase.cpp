// Programmer: Khoi Ho


#include "PickUpBase.h"
#include "Net/UnrealNetwork.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values
APickUpBase::APickUpBase()
{
	// Root Component
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Scene Root"));

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void APickUpBase::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && Age > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(AutoDestroyTimerHandle, this, &APickUpBase::DestroyPickUp, Age, false);
	}
}

void APickUpBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called every frame
void APickUpBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool APickUpBase::OnPickedUp_Validate(ACharacterBase* Character)
{
	return true;
}

void APickUpBase::OnPickedUp_Implementation(ACharacterBase* Character)
{
	if (bDestroyedAfterPickup)
	{
		DestroyPickUp();
	}
}

void APickUpBase::DestroyPickUp()
{
	if (HasAuthority())
	{
		Destroy();
	}
}

