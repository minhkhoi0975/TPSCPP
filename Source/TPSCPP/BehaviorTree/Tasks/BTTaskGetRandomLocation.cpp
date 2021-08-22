// Programmer: Khoi Ho


#include "BTTaskGetRandomLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"

EBTNodeResult::Type UBTTaskGetRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	ACharacter* Character = OwnerComp.GetAIOwner()->GetCharacter();

	if (IsValid(BlackBoard) && IsValid(Character))
	{
		BlackBoard->SetValueAsVector(BlackBoardKey.SelectedKeyName, UNavigationSystemV1::GetRandomPointInNavigableRadius(GetWorld(), Character->GetActorLocation(), Radius));
	}

	return EBTNodeResult::Succeeded;
}
