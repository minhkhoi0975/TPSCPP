// Programmer: Khoi Ho


#include "BTTaskForgetNoise.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTaskForgetNoise::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();

	BlackBoard->SetValueAsBool(BlackBoardKey_bCanHearStrangeNoise.SelectedKeyName, false);

	return EBTNodeResult::Succeeded;
}
