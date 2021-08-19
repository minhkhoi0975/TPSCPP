// Programmer: Khoi Ho


#include "BTTaskFocusOnActor.h"
#include "Characters/CharacterBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

EBTNodeResult::Type UBTTaskFocusOnActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();

	UObject* BlackBoardValue = BlackBoard->GetValueAsObject(BlackBoardKey.SelectedKeyName);
	AActor* Target = Cast<AActor>(BlackBoardValue);

	if (IsValid(Target))
	{
		AIController->SetFocus(Target);
	}

	return EBTNodeResult::Type::Succeeded;
}
