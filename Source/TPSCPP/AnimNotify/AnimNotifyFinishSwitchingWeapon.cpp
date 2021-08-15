// Programmer: Khoi Ho

#include "AnimNotifyFinishSwitchingWeapon.h"
#include "Characters/CharacterBase.h"
#include "Weapons/WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"

void UAnimNotifyFinishSwitchingWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// Validate get the character.
	ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (IsValid(Character) && Character->HasAuthority())
	{
		Character->CharacterFlags &= ~GetCharacterFlag(ECharacterFlags::SwitchingWeapon);
	}
}

