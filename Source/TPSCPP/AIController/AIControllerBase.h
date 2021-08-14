// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	AAIControllerBase();
	
public:
	/** AI*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class UAIPerceptionComponent* AIPerception;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class UBehaviorTree* BehaviorTree;

protected:
	virtual void BeginPlay() override;
};
