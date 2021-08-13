// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyRemoveMagazine.h"
#include "CharacterBase.h"
#include "WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"

void UAnimNotifyRemoveMagazine::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// Validate get the character.
	ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner());
	if (IsValid(Character) && Character->HasAuthority())
	{
		AWeaponBase* Weapon = Character->WeaponCurrent;
		if (IsValid(Weapon))
		{
			Weapon->AmmoInventory += Weapon->AmmoMagazine;
			Weapon->AmmoMagazine = 0;
		}
	}
}

