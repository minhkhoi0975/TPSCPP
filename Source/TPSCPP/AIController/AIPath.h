// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIPath.generated.h"

/**
 *	Types of AI path
 */
UENUM(BlueprintType)
enum class EAIPathType : uint8
{
	// When the NPC stops at the last waypoint, it keeps staying at that waypoint.
	OneWay         UMETA(DisplayName = "OneWay"),   
	// When the NPC stops at the last waypoint, it travels along the path in reverse direction.
	Reverse        UMETA(DisplayName = "Reverse"),  
	// When the NPC stops at the last waypoint, it immediately travels to the first waypoint.
	Circle         UMETA(DisplayName = "Circle")        
};

UCLASS()
class TPSCPP_API AAIPath : public AActor
{
	GENERATED_BODY()

public:
	/** Positions that the NPC moves to.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (MakeEditWidget = true))
	TArray<FVector> Waypoints;

	/** How long the NPC waits before it moves to the next waypoint.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> WaitPeriods;

	/** The type of the waypoint.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAIPathType Type;
	
public:	
	// Sets default values for this actor's properties
	AAIPath();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
