// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskGetRandomLocation.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API UBTTaskGetRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector BlackBoardKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
