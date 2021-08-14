// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotifyInsertBulletShotgun.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API UAnimNotifyInsertBulletShotgun : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FName ReloadLoopStartSectionName = "ReloadLoopStarts";
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
