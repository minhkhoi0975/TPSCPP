// Programmer: Khoi Ho


#include "BTTaskGetCurrentWaypoint.h"
#include "Characters/CharacterBase.h"
#include "AIController/AIPath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"

EBTNodeResult::Type UBTTaskGetCurrentWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	ACharacterBase* Character = Cast<ACharacterBase>(OwnerComp.GetAIOwner()->GetCharacter());

	if (IsValid(BlackBoard) && IsValid(Character))
	{
		AAIPath* AIPath = Character->AIPath;
		if (IsValid(AIPath))
		{
			int WaypointIndex = BlackBoard->GetValueAsInt(BlackBoardKey_WaypointIndex.SelectedKeyName);

			// Get the position of the waypoint.
			FVector WaypointLocationRelative = AIPath->Waypoints[WaypointIndex];
			FVector WaypointLocationWorld = UKismetMathLibrary::TransformLocation(AIPath->GetActorTransform(), WaypointLocationRelative);
			BlackBoard->SetValueAsVector(BlackBoardKey_Waypoint.SelectedKeyName, WaypointLocationWorld);

			// Get the wait period of the waypoint.
			BlackBoard->SetValueAsFloat(BlackBoardKey_WaypointWaitPeriod.SelectedKeyName, AIPath->WaitPeriods[WaypointIndex]);
		}
	}

	return EBTNodeResult::Succeeded;
}
