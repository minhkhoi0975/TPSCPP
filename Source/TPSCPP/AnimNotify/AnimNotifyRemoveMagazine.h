// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotifyRemoveMagazine.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API UAnimNotifyRemoveMagazine : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
