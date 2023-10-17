// Programmer: Khoi Ho


#include "AIControllerBase.h"
#include "AIPath.h"
#include "Characters/CharacterBase.h"
#include "Engine/Engine.h"
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
#include "UObject/ConstructorHelpers.h"

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
	// Check if the actor is a character. If it does, get the half height of the capsule component.
	if (Actor->IsA(ACharacterBase::StaticClass()))
	{
		return Actor->GetActorLocation() + FVector(0.0f, 0.0f, 88.0f);
	}

	// Otherwise, return the location of the character's pivot point.
	return Actor->GetActorLocation();
}

void AAIControllerBase::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "I sense something...");

	// Get character controlled by this AI controller.
	ACharacterBase* ControlledCharacter = Cast<ACharacterBase>(GetCharacter());
	if (!IsValid(ControlledCharacter))
	{
		return;
	}


	UBlackboardComponent* BlackBoardComponent = GetBlackboardComponent();
	if (IsValid(BlackBoardComponent) && IsValid(Actor))
	{
		// Check if the actor is a character.
		ACharacterBase* OtherCharacter = Cast<ACharacterBase>(Actor);
		if (IsValid(OtherCharacter))
		{
			switch (ControlledCharacter->GetAttitudeTowardsCharacter(OtherCharacter))
			{
			case EFactionAttitude::Enemy:		
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

				//UE_LOG(LogTemp, Display, TEXT("Enemy Found."));
				break;

			case EFactionAttitude::Ally:
				//UE_LOG(LogTemp, Display, TEXT("Ally Found."));
				break;

			default:
				//UE_LOG(LogTemp, Display, TEXT("Neutral Found."));
				break;
			}
			
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Error: Blackboard component not found");
	}
}
