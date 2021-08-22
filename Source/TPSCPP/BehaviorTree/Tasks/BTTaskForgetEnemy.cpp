// Programmer: Khoi Ho


#include "BTTaskForgetEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTaskForgetEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	
	BlackBoard->SetValueAsBool(BlackBoardKey_CanSeeEnemy.SelectedKeyName, false);
	BlackBoard->SetValueAsObject(BlackBoardKey_SeenEnemy.SelectedKeyName, nullptr);

	return EBTNodeResult::Succeeded;
}
