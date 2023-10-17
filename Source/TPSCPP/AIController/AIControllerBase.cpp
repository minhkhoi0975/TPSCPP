// Programmer: Khoi Ho


#include "AIControllerBase.h"
#include "AIPath.h"
#include "Characters/CharacterBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception//AISenseConfig_Damage.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AIPerceptionSystem.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/FocalPoint.h"

AAIControllerBase::AAIControllerBase(): Super()
{
	// Configure AI Perception
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI_Perception"));

	// Add AI Sight
	AISightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	AISightConfig->SightRadius = 5000.0f;
	AISightConfig->LoseSightRadius = 6000.0f;
	AISightConfig->PeripheralVisionAngleDegrees = 90.0f;
	AISightConfig->DetectionByAffiliation.bDetectEnemies = true;
	AISightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	AISightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerception->ConfigureSense(*AISightConfig);

	// Add AI Heaing
	AIHearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	AIHearingConfig->HearingRange = 5000.0f;
	AIHearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	AIHearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	AIHearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerception->ConfigureSense(*AIHearingConfig);

	// Add AI Damage
	AIDamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage Config"));
	AIPerception->ConfigureSense(*AIDamageConfig);

	// Set dominant sense
	AIPerception->SetDominantSense(AISightConfig->GetSenseImplementation());

	// Set behavior tree
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BehaviorTreeAsset(TEXT("/Game/MyAssets/AI/AIBehaviorTree"));
	if (BehaviorTreeAsset.Succeeded())
	{
		BehaviorTree = BehaviorTreeAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot find /Game/MyAssets/AI/AIBehaviorTree"));
	}

	// OnTargetPerceptionUpdated event
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AAIControllerBase::OnTargetPerceptionUpdated);
}

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();

	// Run the behavior tree.
	if (IsValid(BehaviorTree) && RunBehaviorTree(BehaviorTree))
	{
		ACharacterBase* ControlledCharacter = Cast<ACharacterBase>(GetPawn());
		if (IsValid(ControlledCharacter))
		{
			AAIPath* AIPath = ControlledCharacter->AIPath;

			if (IsValid(AIPath))
			{
				UBlackboardComponent* BlackBoard = GetBlackboardComponent();
				
				BlackBoard->SetValueAsEnum("PatrolPathType", (uint8)AIPath->Type);
				BlackBoard->SetValueAsInt("PatrolPathDirection", 1);
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("AI path is not set."));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot run behavior tree."));
	}
}

FVector AAIControllerBase::GetFocalPointOnActor(const AActor* Actor) const
{
	if (!Actor)
		return FAISystem::InvalidLocation;

	// If the actor has a focal point, get its location.
	if (const IFocalPoint* FocalPoint = Cast<IFocalPoint>(Actor))
		return FocalPoint->GetFocalPoint();

	// Otherwise, return the root location of the actor.
	return Actor->GetActorLocation();
}

void AAIControllerBase::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor)
		return;

	UBlackboardComponent* BlackBoardComponent = GetBlackboardComponent();
	if (!BlackBoardComponent)
		return;

	ETeamAttitude::Type TeamAttitude = FGenericTeamId::GetAttitude(this, Actor);
	switch (TeamAttitude)
	{
	case ETeamAttitude::Hostile:
		if (UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus) == UAISense_Sight::StaticClass())
		{
			BlackBoardComponent->SetValueAsObject("SeenEnemy", Actor);
			BlackBoardComponent->SetValueAsVector("SeenEnemyLastKnownLocation", Stimulus.StimulusLocation);
			BlackBoardComponent->SetValueAsBool("bCanSeeEnemy", Stimulus.WasSuccessfullySensed());
		}
		else if (UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus) == UAISense_Hearing::StaticClass())
		{
			BlackBoardComponent->SetValueAsVector("StrangeNoiseLocation", Stimulus.StimulusLocation);
			BlackBoardComponent->SetValueAsBool("bCanHearStrangeNoise", Stimulus.WasSuccessfullySensed());
		}
		break;

	case ETeamAttitude::Friendly:
		break;

	default:
		break;
	}
}

FGenericTeamId AAIControllerBase::GetGenericTeamId() const
{
	IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(GetPawn());
	if (!TeamAgent)
		return FGenericTeamId::NoTeam;

	return TeamAgent->GetGenericTeamId();
}
