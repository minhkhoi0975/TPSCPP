// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CharacterAIController.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TPSCPP_API ACharacterAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACharacterAIController();

public:
	/** AI*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class UAIPerceptionComponent* AIPerception;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class UBehaviorTree* BehaviorTree;

protected:
	virtual void BeginPlay() override;
};
