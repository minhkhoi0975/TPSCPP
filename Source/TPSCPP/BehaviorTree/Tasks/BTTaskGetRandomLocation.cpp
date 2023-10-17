// Programmer: Khoi Ho


#include "BTTaskGetRandomLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"

EBTNodeResult::Type UBTTaskGetRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if (!BlackBoard)
		return EBTNodeResult::Failed;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
		return EBTNodeResult::Failed;

	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavigationSystem)
		return EBTNodeResult::Failed;

	FNavLocation ResultLocation;
	bool bCanGetLocation = NavigationSystem->GetRandomPointInNavigableRadius(ControlledPawn->GetActorLocation(), Radius, ResultLocation);

	if (!bCanGetLocation)
		return EBTNodeResult::Failed;

	/*
	ACharacter* Character = OwnerComp.GetAIOwner()->GetCharacter();

	if (IsValid(BlackBoard) && IsValid(Character))
	{
		BlackBoard->SetValueAsVector(BlackBoardKey.SelectedKeyName, UNavigationSystemV1::GetRandomPointInNavigableRadius(GetWorld(), Character->GetActorLocation(), Radius));
	}
	*/

	BlackBoard->SetValueAsVector(BlackBoardKey.SelectedKeyName, ResultLocation.Location);
	return EBTNodeResult::Succeeded;
}
