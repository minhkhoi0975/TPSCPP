// Programmer: Khoi Ho


#include "CharacterSpawner.h"

// Sets default values
ACharacterSpawner::ACharacterSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACharacterSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

