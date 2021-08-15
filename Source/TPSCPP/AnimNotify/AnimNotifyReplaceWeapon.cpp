// Programmer: Khoi Ho


#include "AnimNotifyReplaceWeapon.h"
#include "Characters/CharacterBase.h"
#include "Weapons/WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"

void UAnimNotifyReplaceWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// Validate get the character.
	ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (IsValid(Character) && Character->HasAuthority())
	{
		AWeaponBase* Weapon = Character->WeaponCurrent;
		if (IsValid(Weapon))
		{
			// Destroy the previous weapon.
			Character->WeaponCurrent->Destroy();
		}

		// Spawn a new weapon.
		FWeaponInstance CurrentWeaponInstance = Character->Inventory[Character->WeaponCurrentIndex];
		Character->SpawnWeapon(CurrentWeaponInstance.WeaponClass, CurrentWeaponInstance.AmmoMagazine, CurrentWeaponInstance.AmmoInventory);
	}
}

