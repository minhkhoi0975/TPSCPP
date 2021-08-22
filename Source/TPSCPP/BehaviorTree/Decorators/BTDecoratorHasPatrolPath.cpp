// Programmer: Khoi Ho


#include "BTDecoratorHasPatrolPath.h"
#include "Characters/CharacterBase.h"
#include "AIController.h"

bool UBTDecoratorHasPatrolPath::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	ACharacterBase* Character = Cast<ACharacterBase>(OwnerComp.GetAIOwner()->GetCharacter());
	if (IsValid(Character) && IsValid(Character->AIPath))
	{
		return true;
	}

	return false;
}
