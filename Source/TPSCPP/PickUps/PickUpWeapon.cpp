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
				Weapon->EnableSimulatePhysics();
			}
			else
			{
				Weapon->DisableSimulatePhysics();
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
	DOREPLIFETIME(APickUpWeapon, bWeaponMeshSimulatesPhysics);
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

void APickUpWeapon::OnInteracted(ACharacterBase* Character)
{
	if (HasAuthority() && IsValid(Character))
	{
		for (int i = 0; i < Character->Inventory.Num(); i++)
		{
			if (!(Character->Inventory[i].WeaponClass))
			{
				Character->Inventory[i].WeaponClass = WeaponInstance.WeaponClass;
				Character->Inventory[i].AmmoMagazine = WeaponInstance.AmmoMagazine;
				Character->Inventory[i].AmmoInventory = WeaponInstance.AmmoInventory;

				Character->SwitchWeapon(i);

				if (bDestroyedAfterPickup)
				{
					DestroyPickUp();
				}

				return;
			}		
		}
	}
}

