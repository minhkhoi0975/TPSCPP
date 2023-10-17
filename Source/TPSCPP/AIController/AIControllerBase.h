// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception//AISenseConfig_Damage.h"
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

protected:
	/**AI sense configs*/
	UPROPERTY()
	UAISenseConfig_Sight* AISightConfig;

	UPROPERTY()
	UAISenseConfig_Hearing* AIHearingConfig;

	UPROPERTY()
	UAISenseConfig_Damage* AIDamageConfig;
	
public:
	/** AI*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class UAIPerceptionComponent* AIPerception;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
	class UBehaviorTree* BehaviorTree;

protected:
	virtual void BeginPlay() override;

	virtual FVector GetFocalPointOnActor(const AActor* Actor) const override;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

public:
	virtual FGenericTeamId GetGenericTeamId() const override;
};
