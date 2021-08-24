// Programmer: Khoi Ho


#include "CharacterSpawner.h"
#include "CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
ACharacterSpawner::ACharacterSpawner(): Super()
{
	// Dummy.
	Dummy = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Dummy"));
	Dummy->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);
	Dummy->bHiddenInGame = true;
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(TEXT("/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin"));
	if (SkeletalMesh.Succeeded())
	{
		Dummy->SetSkeletalMesh(SkeletalMesh.Object);
	}

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// No need to load this on client.
	bNetLoadOnClient = false;
}

// Called when the game starts or when spawned
void ACharacterSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnNPC();
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ACharacterSpawner::SpawnNPC, Cooldown, true);
}

void ACharacterSpawner::SpawnNPC()
{
	APawn* Pawn = UAIBlueprintHelperLibrary::SpawnAIFromClass(GetWorld(), CharacterClass, BehaviorTree, GetActorLocation(), GetActorRotation(), true);

	ACharacterBase* Character = Cast<ACharacterBase>(Pawn);
	if (IsValid(Character))
	{
		Character->AIPath = AIPath;

		UBlackboardComponent* BlackBoard = UAIBlueprintHelperLibrary::GetBlackboard(Character);
		if (IsValid(BlackBoard))
		{
			BlackBoard->SetValueAsEnum("PatrolPathType", (uint8)AIPath->Type);
			BlackBoard->SetValueAsInt("PatrolPathDirection", 1);
		}
	}
}

