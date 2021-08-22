// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterSpawner.generated.h"

UCLASS()
class TPSCPP_API ACharacterSpawner : public AActor
{
	GENERATED_BODY()

public:
	/**Used to visualize the spawner. Hidden in game.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* Dummy;

public:
	/**The template of spawned NPCs.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACharacterBase> CharacterClass;

	/** How long before another character is spawned.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Cooldown;

	/** If this variable is set, it will override the default behavior tree of the character.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBehaviorTree* BehaviorTree;

	/** It this variable is set, the spawned NPC will travel along the path. Otherwise, it will wander.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AAIPath* AIPath;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTimerHandle SpawnTimerHandle;
	
public:	
	// Sets default values for this actor's properties
	ACharacterSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// UFUNCTION(BlueprintCallable)
	void SpawnNPC();
};
