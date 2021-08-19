// Programmer: Khoi Ho


#include "BTTaskClearFocus.h"
#include "AIController.h"

EBTNodeResult::Type UBTTaskClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetAIOwner()->ClearFocus(EAIFocusPriority::Gameplay);

	return EBTNodeResult::Type::Succeeded;
}