// Programmer: Khoi Ho


#include "PickUpWeapon.h"
#include "Weapons/WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

APickUpWeapon::APickUpWeapon(): Super()
{
}

void APickUpWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (HasAuthority() && WeaponInstance.WeaponClass)
	{
		if (IsValid(Weapon))
		{
			Weapon->Destroy();
		}

		Weapon = GetWorld()->SpawnActorDeferred<AWeaponBase>(WeaponInstance.WeaponClass, GetActorTransform(), this, nullptr);
		if (IsValid(Weapon))
		{
			Weapon->AmmoMagazine = (WeaponInstance.AmmoMagazine >= 0 && WeaponInstance.AmmoMagazine <= Weapon->AmmoMagazineMax) ? WeaponInstance.AmmoMagazine : Weapon->AmmoMagazineMax;
			Weapon->AmmoInventory = (WeaponInstance.AmmoInventory >= 0 && WeaponInstance.AmmoInventory <= (Weapon->AmmoMax - Weapon->AmmoMagazine)) ? WeaponInstance.AmmoInventory : Weapon->AmmoMax - Weapon->AmmoMagazine;

			if (bWeaponMeshSimulatesPhysics)
			{
				Weapon->GunMesh->SetSimulatePhysics(true);
				Weapon->GunMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
				Weapon->GunMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
				Weapon->GunMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); // Don't let the character move the weapon.
			}
			else
			{
				Weapon->GunMesh->SetSimulatePhysics(false);
			}

			FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
			Weapon->AttachToActor(this, AttachmentTransformRules);

			Weapon->FinishSpawning(GetActorTransform());
		}
	}
}

void APickUpWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickUpWeapon, Weapon);
}

void APickUpWeapon::DestroyPickUp()
{
	if (HasAuthority())
	{
		if (IsValid(Weapon))
		{
			Weapon->Destroy();
		}
		Destroy();
	}
}

