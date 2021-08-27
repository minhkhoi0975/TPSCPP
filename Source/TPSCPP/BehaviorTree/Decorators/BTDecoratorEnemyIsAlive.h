// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BTDecoratorEnemyIsAlive.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API UBTDecoratorEnemyIsAlive : public UBTDecorator
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector BlackBoardKey_SeenEnemy;

public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
