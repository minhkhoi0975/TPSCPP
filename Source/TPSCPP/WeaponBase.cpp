// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
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

bool AWeaponBase::Fire_Validate()
{
	return true;
}

void AWeaponBase::Fire_Implementation()
{
	// Check if the gun is carried by a character.
	ACharacterBase* CarryingCharacter = nullptr;
	AActor* Instigator = GetInstigator();
	if (IsValid(Instigator))
	{
		CarryingCharacter = Cast<ACharacterBase>(Instigator);	
	}

	FVector StartLocation;
	FVector FiringDirection;
	FVector EndLocation;

	// If the gun is being carried by a player character, line trace from camera.
	if (IsValid(CarryingCharacter) && Cast<APlayerController>(CarryingCharacter->GetController()))
	{
		StartLocation = CarryingCharacter->Camera->GetComponentLocation();
		FiringDirection = CarryingCharacter->Camera->GetForwardVector();
		EndLocation = StartLocation + FiringDirection * 1000000;
	}
	// If the gun has no carrying character or is being carried by an NPC, line trace from muzzle.
	else
	{
		StartLocation = Muzzle->GetComponentLocation();
		FiringDirection = GetActorForwardVector();
		EndLocation = StartLocation + FiringDirection * 1000000;
	}

	// When line tracing, ignore the shooter and the gun itself.
	FCollisionQueryParams CollisionQuerryParams;
	CollisionQuerryParams.AddIgnoredActor(this);
	CollisionQuerryParams.AddIgnoredActor(GetOwner());

	// Line trace.
	FHitResult HitResult;
	bool Hit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, CollisionQuerryParams);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 5.0f);
	if (Hit)
	{
		// TODO: Apply damage to hit actor.

		FVector ImpactLocation = HitResult.ImpactPoint;
		FRotator ImpactRotator = FRotationMatrix::MakeFromX(HitResult.ImpactNormal).Rotator();
		
		FTransform ImpactTransform = FTransform(ImpactRotator, ImpactLocation);
		PlayImpactEffect(ImpactTransform);
	}

	PlayShootingSound();
}

bool AWeaponBase::IsGunOutOfAmmo()
{
	return AmmoMagazine <= 0;
}

bool AWeaponBase::PlayShootingSound_Validate()
{
	return true;
}

void AWeaponBase::PlayShootingSound_Implementation()
{
	// Play shooting sound.
	if (SoundFire)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundFire, GetActorLocation());
	}
}

bool AWeaponBase::PlayImpactEffect_Validate(const FTransform& SpawnTransform)
{
	return true;
}

void AWeaponBase::PlayImpactEffect_Implementation(const FTransform& SpawnTransform)
{
	if (EffectImpact)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EffectImpact, SpawnTransform);
	}
}

