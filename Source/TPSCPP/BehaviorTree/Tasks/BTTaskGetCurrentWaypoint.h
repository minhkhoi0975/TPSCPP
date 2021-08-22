// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskGetCurrentWaypoint.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API UBTTaskGetCurrentWaypoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector BlackBoardKey_WaypointIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector BlackBoardKey_Waypoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector BlackBoardKey_WaypointWaitPeriod;
	
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
