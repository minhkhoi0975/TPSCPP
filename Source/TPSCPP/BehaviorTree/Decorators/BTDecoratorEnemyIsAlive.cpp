// Programmer: Khoi Ho


#include "BTDecoratorEnemyIsAlive.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CharacterBase.h"

bool UBTDecoratorEnemyIsAlive::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if (IsValid(BlackBoard))
	{
		ACharacterBase* Character = Cast<ACharacterBase>(BlackBoard->GetValueAsObject(BlackBoardKey_SeenEnemy.SelectedKeyName));
		if (IsValid(Character) && Character->HealthCurrent > 0)
		{
			return true;
		}
	}

	return false;
}
