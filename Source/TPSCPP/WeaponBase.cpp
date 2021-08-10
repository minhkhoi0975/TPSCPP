// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Gun mesh
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Mesh"));

	// Muzzle
	Muzzle = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("Muzzle"));
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBase, AmmoMagazine);
	DOREPLIFETIME(AWeaponBase, AmmoInventory);
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AWeaponBase::StartFiring_Validate()
{
	return true;
}

void AWeaponBase::StartFiring_Implementation()
{
	Fire();
}

bool AWeaponBase::StopFiring_Validate()
{
	return true;
}

void AWeaponBase::StopFiring_Implementation()
{
}

void AWeaponBase::Fire_Implementation()
{
	AActor* Instigator = GetInstigator();
	if (IsValid(Instigator))
	{
		ACharacterBase* CarryingCharacter = Cast<ACharacterBase>(Instigator);
		if (IsValid(CarryingCharacter))
		{
			FVector StartLocation = CarryingCharacter->Camera->GetComponentLocation();
			FVector FiringDirection = CarryingCharacter->Camera->GetForwardVector();
			FVector EndLocation = StartLocation + FiringDirection * 1000000;

			FHitResult HitResult;

			FCollisionQueryParams CollisionQuerryParams;
			CollisionQuerryParams.AddIgnoredActor(this);
			CollisionQuerryParams.AddIgnoredActor(GetInstigator());

			GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, CollisionQuerryParams);
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 5.0f);
		}
	}
}

bool AWeaponBase::IsGunOutOfAmmo()
{
	return AmmoMagazine <= 0;
}

