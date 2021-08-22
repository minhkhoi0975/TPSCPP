// Programmer: Khoi Ho


#include "AIPath.h"

// Sets default values
AAIPath::AAIPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// No need to load this on client.
	bNetLoadOnClient = false;
}

#if WITH_EDITOR
void AAIPath::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Resize WaitPeriods to match the size of Waypoints.
	WaitPeriods.SetNum(Waypoints.Num());
}
#endif

