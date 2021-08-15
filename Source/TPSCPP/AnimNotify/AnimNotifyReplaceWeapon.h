// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotifyReplaceWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API UAnimNotifyReplaceWeapon : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
