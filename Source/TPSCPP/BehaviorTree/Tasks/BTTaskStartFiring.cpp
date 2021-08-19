// Programmer: Khoi Ho


#include "BTTaskStartFiring.h"
#include "AIController.h"
#include "Characters/CharacterBase.h"

EBTNodeResult::Type  UBTTaskStartFiring::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	ACharacterBase* Character = Cast<ACharacterBase>(AIController->GetCharacter());
	if (IsValid(Character))
	{
		Character->StartFiring();
	}

	return EBTNodeResult::Succeeded;
}
