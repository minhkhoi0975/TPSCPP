// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "PickUps/PickUpBase.h"
#include "PickUpWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API APickUpWeapon : public APickUpBase
{
	GENERATED_BODY()

public:
	APickUpWeapon();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pick-up")
	struct FWeaponInstance WeaponInstance;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Pick-up")
	class AWeaponBase* Weapon;

	/**Does the weapon mesh simulate physics?*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pick-up")
	bool bWeaponMeshSimulatesPhysics;

public:
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Required for replication
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void DestroyPickUp() override;
};
