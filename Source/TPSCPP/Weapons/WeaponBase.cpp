// Programmer: Khoi Ho


#include "WeaponBase.h"
#include "Characters/CharacterBase.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"
#include "Camera/CameraComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Curves/CurveFloat.h"
#include "DrawDebugHelpers.h"
#include "Engine/EngineTypes.h"
#include "Perception/AISense_Hearing.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Gun mesh
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Mesh"));
	GunMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	GunMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	GunMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Overlap);
	GunMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	RootComponent = GunMesh;

	// Muzzle
	Muzzle = CreateDefaultSubobject<USphereComponent>(TEXT("Muzzle"));

	// Enable replication
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	// Recoil Timeline
	if (HasAuthority() && IsValid(RecoilCurve))
	{
		
		RecoilTimelineCallback.BindUFunction(this, FName("TimelineRecoil_Update"));

		RecoilTimeline.AddInterpFloat(RecoilCurve, RecoilTimelineCallback);
		// UE_LOG(LogTemp, Display, TEXT("Recoil timeline is set."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("This weapon has no recoil curve."));
	}
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBase, AmmoMagazine);
	DOREPLIFETIME(AWeaponBase, AmmoInventory);
	DOREPLIFETIME(AWeaponBase, bFiring);
	DOREPLIFETIME(AWeaponBase, WeaponFlags);

	DOREPLIFETIME(AWeaponBase, GunMeshLocation);
	DOREPLIFETIME(AWeaponBase, GunMeshRotation);
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ReplicateSkeletalMeshMovement();

	RecoilTimeline.TickTimeline(DeltaTime);
}

void AWeaponBase::TimelineRecoil_Update()
{
	ApplyRecoil();
}

bool AWeaponBase::StartFiring_Validate()
{
	return true;
}

void AWeaponBase::StartFiring_Implementation()
{
	WeaponFlags |= GetWeaponFlag(EWeaponFlags::Firing);

	// Clear the timer to avoid overlapping function calls.
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);

	Fire();
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AWeaponBase::Fire, FireRate, true);
}

bool AWeaponBase::ApplyRecoil_Validate()
{
	return true;
}

void AWeaponBase::ApplyRecoil_Implementation()
{
	ACharacterBase* CarryingCharacter = Cast<ACharacterBase>(GetInstigator());
	if (!CarryingCharacter)
		return;

	CarryingCharacter->ApplyRecoil(RecoilTimeline.GetPlaybackPosition() * RecoilScale);
}

bool AWeaponBase::StopFiring_Validate()
{
	return true;
}

void AWeaponBase::StopFiring_Implementation()
{
	WeaponFlags &= ~GetWeaponFlag(EWeaponFlags::Firing);
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

bool AWeaponBase::Fire_Validate()
{
	return true;
}

void AWeaponBase::Fire_Implementation()
{
	if (!IsCurrentMagazineEmpty() && !(WeaponFlags & GetWeaponFlag(EWeaponFlags::Reloading)))
	{
		// Abort this function if the carrying character is switching weapon.
		ACharacterBase* CarryingCharacter = Cast<ACharacterBase>(GetInstigator());
		if (CarryingCharacter && CarryingCharacter->CharacterFlags & GetCharacterFlag(ECharacterFlags::SwitchingWeapon))
			return;

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

			// UE_LOG(LogTemp, Display, TEXT("Shoot from camera."));
		}
		// If the gun has no carrying character or is being carried by an NPC, line trace from muzzle.
		else
		{
			// Find the starting point of line trace.
			StartLocation = GunMesh->GetSocketLocation("Muzzle");

			// Get forward vector and "bend" it to become firing direction.
			FiringDirection = GetActorRightVector();
			FiringDirection = UKismetMathLibrary::RandomUnitVectorInConeInRadians(FiringDirection, BulletSpreadAngleRad);

			// Find end point.
			EndLocation = StartLocation + FiringDirection * 1000000;

			// UE_LOG(LogTemp, Display, TEXT("Shoot from muzzle."));
		}

		// When line tracing, ignore the shooter and the gun itself.
		FCollisionQueryParams CollisionQuerryParams;
		CollisionQuerryParams.AddIgnoredActor(this);
		CollisionQuerryParams.AddIgnoredActor(GetOwner());

		// Line trace.
		FHitResult HitResult;
		bool Hit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, CollisionQuerryParams);
		//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 5.0f);
		if (Hit)
		{
			// Apply damage to the hit actor.
			AActor* HitActor = HitResult.GetActor();
			if (IsValid(HitActor))
			{
				// Get the controller of the character that fires this weapon.
				UGameplayStatics::ApplyPointDamage(HitActor, DamageBase, FiringDirection, HitResult, GetInstigatorController(), this, TSubclassOf<UDamageType>());
			}

			// IF the hit actor is simulating physics, add impulse to it.
			UPrimitiveComponent* HitComponent = HitResult.GetComponent();
			if (IsValid(HitComponent) && HitComponent->IsSimulatingPhysics())
			{
				FVector Impulse = FiringDirection / FiringDirection.Size() * 300.0f;
				HitComponent->AddImpulse(Impulse, HitResult.BoneName, true);
			}

			// Spawn impact particle effect.
			FVector ImpactLocation = HitResult.ImpactPoint;
			FRotator ImpactRotator = FRotationMatrix::MakeFromX(HitResult.ImpactNormal).Rotator();
			FTransform ImpactTransform = FTransform(ImpactRotator, ImpactLocation);
			PlayEffect(EffectImpact, ImpactTransform);
		}

		// Reduce ammo
		ReduceAmmo();

		// Recoil and camera shake.
		RecoilTimeline.PlayFromStart();
		if (IsValid(CarryingCharacter))
		{
			APlayerController* PlayerController = Cast<APlayerController>(CarryingCharacter->GetController());
			if (IsValid(PlayerController) && IsValid(CameraShakeClass))
			{
				PlayerController->ClientStartCameraShake(CameraShakeClass, CameraShakeScale);
			}
		}

		// Play shooting sound.
		PlayShootingSound();

		// Report noise event.
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1.0f, IsValid(CarryingCharacter) ? CarryingCharacter : nullptr, 0.0f, FName("Noise"));

		// Play muzzle flash effect.
		FTransform MuzzleFlashTransform = GunMesh->GetSocketTransform("Muzzle");
		PlayEffect(EffectMuzzleFlash, MuzzleFlashTransform, Muzzle);
	}
	else if (CanReload())
	{
		Reload();
	}
}

void AWeaponBase::ReduceAmmo()
{
	if (HasAuthority())
	{
		AmmoMagazine -= AmmoPerShot;
	}
}

bool AWeaponBase::IsCurrentMagazineEmpty()
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

bool AWeaponBase::Reload_Validate()
{
	return true;
}

void AWeaponBase::Reload_Implementation()
{
	ACharacterBase* CarryingCharacter = Cast<ACharacterBase>(GetInstigator());
	if (!CarryingCharacter)
		return;

	if (IsValid(AnimMontageReload) && CanReload())
		CarryingCharacter->ReplicateAnimMontagePlay(AnimMontageReload);
	else
		UE_LOG(LogTemp, Warning, TEXT("The weapon has no reload animation montage."));
}

bool AWeaponBase::CanReload()
{
	return AmmoInventory > 0 && AmmoMagazine < AmmoMagazineMax && !(WeaponFlags & GetWeaponFlag(EWeaponFlags::Reloading));
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

void AWeaponBase::ReplicateSkeletalMeshMovement()
{
	if (GunMesh->IsSimulatingPhysics())
	{
		if (HasAuthority())
		{
			GunMeshLocation = GunMesh->GetComponentLocation();
			GunMeshRotation = GunMesh->GetComponentRotation();
		}
		else
		{
			GunMesh->SetWorldLocation(GunMeshLocation, false, nullptr, ETeleportType::TeleportPhysics);
			GunMesh->SetWorldRotation(GunMeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
		}
	}
}

bool AWeaponBase::EnableSimulatePhysics_Validate()
{
	return true;
}

void AWeaponBase::EnableSimulatePhysics_Implementation()
{
	GunMesh->SetSimulatePhysics(true);
	GunMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GunMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	GunMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); // Don't let the character move the weapon.
}

bool AWeaponBase::DisableSimulatePhysics_Validate()
{
	return true;
}

void AWeaponBase::DisableSimulatePhysics_Implementation()
{
	GunMesh->SetSimulatePhysics(false);
	GunMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	GunMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	GunMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Overlap);
	GunMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

