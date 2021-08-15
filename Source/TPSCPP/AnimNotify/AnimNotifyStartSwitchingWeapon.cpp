// Programmer: Khoi Ho


#include "AnimNotifyStartSwitchingWeapon.h"
#include "Characters/CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"

void UAnimNotifyStartSwitchingWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// Validate get the character.
	ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (IsValid(Character) && Character->HasAuthority())
	{
		Character->CharacterFlags |= GetCharacterFlag(ECharacterFlags::SwitchingWeapon);
	}
}
