// Programmer: Khoi Ho


#include "AnimNotifyInsertBulletShotgun.h"
#include "Characters/CharacterBase.h"
#include "Weapons/WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

void UAnimNotifyInsertBulletShotgun::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// Validate get the character.
	ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (IsValid(Character) && Character->HasAuthority())
	{
		AWeaponBase* Weapon = Character->WeaponCurrent;
		if (IsValid(Weapon))
		{
			Weapon->AmmoMagazine++;
			Weapon->AmmoInventory--;

			if (Weapon->AmmoMagazine < Weapon->AmmoMagazineMax && Weapon->AmmoInventory > 0)
			{
				// Character->CharacterMeshComponent->GetAnimInstance()->Montage_JumpToSection("ReloadLoopStarts", Weapon->AnimMontageReload);
				Character->ReplicateAnimMontageJumpToSection("ReloadLoopStarts", Weapon->AnimMontageReload);
			}
		}
	}
}

