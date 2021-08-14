// Programmer: Khoi Ho


#include "AIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception//AISenseConfig_Damage.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "BehaviorTree/BehaviorTree.h"

AAIControllerBase::AAIControllerBase() : Super()
{
	// Configure AI Perception
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI_Perception"));

	// Add AI Sight
	UAISenseConfig_Sight* AISightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	AISightConfig->SightRadius = 5000.0f;
	AISightConfig->LoseSightRadius = 6000.0f;
	AISightConfig->PeripheralVisionAngleDegrees = 90.0f;
	AISightConfig->DetectionByAffiliation.bDetectEnemies = true;
	AISightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	AISightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerception->ConfigureSense(*AISightConfig);

	// Add AI Heaing
	UAISenseConfig_Hearing* AIHearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	AIHearingConfig->HearingRange = 5000.0f;
	AIHearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	AIHearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
	AIHearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	AIPerception->ConfigureSense(*AIHearingConfig);

	// Add AI Damage
	UAISenseConfig_Damage* AIDamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage Config"));
	AIPerception->ConfigureSense(*AIDamageConfig);

	// Set dominant sense
	AIPerception->SetDominantSense(AISightConfig->GetSenseImplementation());
}

void AAIControllerBase::BeginPlay()
{
	// Run the behavior tree.
	if (IsValid(BehaviorTree))
	{
		RunBehaviorTree(BehaviorTree);
	}
}
