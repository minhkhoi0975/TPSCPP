// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "Interfaces/Interactable.h"
#include "GameFramework/Actor.h"
#include "PickUpBase.generated.h"

UCLASS()
class TPSCPP_API APickUpBase : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USceneComponent* DefaultSceneRoot;
	
public:	
	// Sets default values for this actor's properties
	APickUpBase();

protected:
	FTimerHandle AutoDestroyTimerHandle;

public:
	/**How long before the pick-up is automatically destroyed. If <= 0, the pick-up is not automatically destroyed.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pick-up")
	float Age = -1.0f;

	/**Is this pick-up destroyed after the player interacts with it?.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pick-up")
	bool bDestroyedAfterPickup = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Required for replication
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void DestroyPickUp();

	virtual void OnInteracted(ACharacterBase* Character) override;
};
