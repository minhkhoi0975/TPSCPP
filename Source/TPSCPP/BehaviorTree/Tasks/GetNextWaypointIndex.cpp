// Programmer: Khoi Ho


#include "GetNextWaypointIndex.h"
#include "Characters/CharacterBase.h"
#include "AIController/AIPath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"

EBTNodeResult::Type UGetNextWaypointIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoardComponent = OwnerComp.GetBlackboardComponent();
	ACharacterBase* Character = Cast<ACharacterBase>(OwnerComp.GetAIOwner()->GetCharacter());

	if (IsValid(BlackBoardComponent) && IsValid(Character))
	{
		AAIPath* AIPath = Character->AIPath;
		if (IsValid(AIPath))
		{
			EAIPathType AIPathType = (EAIPathType)BlackBoardComponent->GetValueAsEnum(BlackBoardKey_PathType.SelectedKeyName);
			int PatrolDirection = BlackBoardComponent->GetValueAsInt(BlackBoardKey_PatrolDirection.SelectedKeyName);
			int WaypointIndex = BlackBoardComponent->GetValueAsInt(BlackBoardKey_WaypointIndex.SelectedKeyName);
			
			switch (AIPathType)
			{
			
			case EAIPathType::OneWay:
				if (WaypointIndex + 1 < AIPath->Waypoints.Num())
				{
					WaypointIndex++;
					BlackBoardComponent->SetValueAsInt(BlackBoardKey_WaypointIndex.SelectedKeyName, WaypointIndex);
				}
				break;


			case EAIPathType::Reverse:
				if ((WaypointIndex + 1 == AIPath->Waypoints.Num() && PatrolDirection > 0) || (WaypointIndex == 0 && PatrolDirection < 0))
				{
					PatrolDirection = -PatrolDirection;
					BlackBoardComponent->SetValueAsInt(BlackBoardKey_PatrolDirection.SelectedKeyName, PatrolDirection);
				}
				WaypointIndex += PatrolDirection;
				BlackBoardComponent->SetValueAsInt(BlackBoardKey_WaypointIndex.SelectedKeyName, WaypointIndex);
				break;


			case EAIPathType::Circle:
				if (WaypointIndex + 1 < AIPath->Waypoints.Num())
				{
					WaypointIndex++;
				}
				else
				{
					WaypointIndex = 0;
				}
				BlackBoardComponent->SetValueAsInt(BlackBoardKey_WaypointIndex.SelectedKeyName, WaypointIndex);
				break;
			}
		}
	}

	return EBTNodeResult::Succeeded;
}
