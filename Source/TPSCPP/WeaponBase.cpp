// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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

	// Enable replication
	SetReplicates(true);
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
	DOREPLIFETIME(AWeaponBase, bFiring);
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
	bFiring = true;
	Fire();
}

bool AWeaponBase::StopFiring_Validate()
{
	return true;
}

void AWeaponBase::StopFiring_Implementation()
{
	bFiring = false;
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
		// Find the starting point of line trace.
		StartLocation = CarryingCharacter->Camera->GetComponentLocation();

		// Get forward vector and "bend" it to become firing direction.
		FiringDirection = CarryingCharacter->Camera->GetForwardVector();
		FiringDirection = UKismetMathLibrary::RandomUnitVectorInConeInRadians(FiringDirection, BulletSpreadAngleRad);

		// Find end point.
		EndLocation = StartLocation + FiringDirection * 1000000;

		UE_LOG(LogTemp, Warning, TEXT("Shoot from camera."));
	}
	// If the gun has no carrying character or is being carried by an NPC, line trace from muzzle.
	else
	{
		// Find the starting point of line trace.
		StartLocation = Muzzle->GetComponentLocation();

		// Get forward vector and "bend" it to become firing direction.
		FiringDirection = GetActorForwardVector();
		FiringDirection = UKismetMathLibrary::RandomUnitVectorInConeInRadians(FiringDirection, BulletSpreadAngleRad);

		// Find end point.
		EndLocation = StartLocation + FiringDirection * 1000000;

		UE_LOG(LogTemp, Warning, TEXT("Shoot from muzzle."));
	}

	// When line tracing, ignore the shooter and the gun itself.
	FCollisionQueryParams CollisionQuerryParams;
	CollisionQuerryParams.AddIgnoredActor(this);
	CollisionQuerryParams.AddIgnoredActor(GetOwner());

	// Line trace.
	FHitResult HitResult;
	bool Hit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Camera, CollisionQuerryParams);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 5.0f);
	if (Hit)
	{
		// TODO: Apply damage to hit actor.

		FVector ImpactLocation = HitResult.ImpactPoint;
		FRotator ImpactRotator = FRotationMatrix::MakeFromX(HitResult.ImpactNormal).Rotator();
		
		FTransform ImpactTransform = FTransform(ImpactRotator, ImpactLocation);
		PlayEffect(EffectImpact, ImpactTransform);
	}

	// Play shooting sound.
	PlayShootingSound();

	// Play muzzle flash effect.
	FTransform MuzzleFlashTransform = GunMesh->GetSocketTransform("Muzzle");
	PlayEffect(EffectMuzzleFlash, MuzzleFlashTransform, Muzzle);
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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("The sound is nullptr."));
	}
}

bool AWeaponBase::PlayEffect_Validate(UParticleSystem* ParticleEffect, const FTransform& SpawnTransform, USceneComponent* AttachToComponent)
{
	return true;
}

void AWeaponBase::PlayEffect_Implementation(UParticleSystem* ParticleEffect, const FTransform& SpawnTransform, USceneComponent* AttachToComponent)
{
	if (EffectImpact)
	{
		if (AttachToComponent)
		{
			UGameplayStatics::SpawnEmitterAttached
			(
				ParticleEffect, 
				AttachToComponent, 
				FName("None"), 
				SpawnTransform.GetLocation(), 
				SpawnTransform.Rotator(), 
				SpawnTransform.GetScale3D(), 
				EAttachLocation::Type::KeepWorldPosition
			);
		}
		else
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleEffect, SpawnTransform);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("The effect is nullptr."));
	}
}

