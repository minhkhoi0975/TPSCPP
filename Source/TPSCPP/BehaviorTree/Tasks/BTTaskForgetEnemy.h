// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BTTaskForgetEnemy.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API UBTTaskForgetEnemy : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector BlackBoardKey_SeenEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector BlackBoardKey_CanSeeEnemy;

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
