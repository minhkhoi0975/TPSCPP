// Programmer: Khoi Ho


#include "AnimNotifyInsertMagazine.h"
#include "Characters/CharacterBase.h"
#include "Weapons/WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"

void UAnimNotifyInsertMagazine::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// Validate get the character.
	ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (IsValid(Character) && Character->HasAuthority())
	{
		AWeaponBase* Weapon = Character->WeaponCurrent;
		if (IsValid(Weapon))
		{
			Weapon->AmmoMagazine = Weapon->AmmoInventory >= Weapon->AmmoMagazineMax ? Weapon->AmmoMagazineMax : Weapon->AmmoInventory;
			Weapon->AmmoInventory -= Weapon->AmmoMagazine;
		}
	}
}