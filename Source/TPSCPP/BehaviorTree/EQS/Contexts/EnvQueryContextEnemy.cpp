// Programmer: Khoi Ho


#include "EnvQueryContextEnemy.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "AIController/AIControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

void UEnvQueryContextEnemy::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	AAIControllerBase* AIController = Cast<AAIControllerBase>(Cast<AActor>((QueryInstance.Owner).Get())->GetInstigatorController());
	if (IsValid(AIController))
	{
		UBlackboardComponent* BlackBoard = AIController->GetBlackboardComponent();
		if (IsValid(BlackBoard))
		{
			ACharacter* Enemy = Cast<ACharacter>(BlackBoard->GetValueAsObject("SeenEnemy"));
			if (IsValid(Enemy))
			{
				UEnvQueryItemType_Actor::SetContextHelper(ContextData, Enemy);
			}

			//UE_LOG(LogTemp, Display, TEXT("Enemy context"));
		}
	}
}