// Programmer: Khoi Ho


#include "BTTaskStopFiring.h"
#include "AIController.h"
#include "Characters/CharacterBase.h"

EBTNodeResult::Type  UBTTaskStopFiring::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	ACharacterBase* Character = Cast<ACharacterBase>(AIController->GetCharacter());
	if (IsValid(Character))
	{
		Character->StopFiring();
	}

	return EBTNodeResult::Succeeded;
}
