// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotifyInsertMagazine.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API UAnimNotifyInsertMagazine : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
