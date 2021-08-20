// Programmer: Khoi Ho


#include "BTTaskMoveToLocation.h"
#include "Characters/CharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

EBTNodeResult::Type UBTTaskMoveToLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();

	FVector Location = BlackBoard->GetValueAsVector(BlackBoardKey.SelectedKeyName);

	AIController->MoveToLocation(Location);

	return EBTNodeResult::Type::Succeeded;
}
