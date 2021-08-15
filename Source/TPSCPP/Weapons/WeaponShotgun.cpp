// Programmer: Khoi Ho


#include "WeaponShotgun.h"
#include "Characters/CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

void AWeaponShotgun::Fire_Implementation()
{
	if (!IsCurrentMagazineEmpty() && !(WeaponFlags & GetWeaponFlag(EWeaponFlags::Reloading)))
	{
		// Check if the gun is carried by a character.
		ACharacterBase* CarryingCharacter = nullptr;
		AActor* Instigator = GetInstigator();
		if (IsValid(Instigator))
		{
			CarryingCharacter = Cast<ACharacterBase>(Instigator);
		}

		// Abort this function if the carrying character is switching weapon.
		if (IsValid(CarryingCharacter) && (CarryingCharacter->CharacterFlags & GetCharacterFlag(ECharacterFlags::SwitchingWeapon)))
		{
			return;
		}

		FVector StartLocation;
		FVector FiringDirection;
		FVector EndLocation;

		for (int i = 0; i < ShellsPerShot; i++)
		{
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

				//UE_LOG(LogTemp, Display, TEXT("Shoot from camera."));
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

				//UE_LOG(LogTemp, Display, TEXT("Shoot from muzzle."));
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
				PlayEffect(EffectImpact, ImpactTransform);
			}
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
				PlayerController->ClientPlayCameraShake(CameraShakeClass, CameraShakeScale);
			}
		}

		// Play shooting sound.
		PlayShootingSound();

		// Play muzzle flash effect.
		FTransform MuzzleFlashTransform = GunMesh->GetSocketTransform("Muzzle");
		PlayEffect(EffectMuzzleFlash, MuzzleFlashTransform, Muzzle);
	}
	else if (CanReload())
	{
		Reload();
	}
}
