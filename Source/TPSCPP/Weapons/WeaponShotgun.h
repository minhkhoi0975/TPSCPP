// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponShotgun.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API AWeaponShotgun : public AWeaponBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int ShellsPerShot = 6;
	
public:
	virtual void Fire_Implementation() override;
};
