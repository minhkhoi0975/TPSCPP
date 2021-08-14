// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraShake.h"
#include "WeaponBase.generated.h"

#define GetWeaponFlag(WeaponFlag) static_cast<uint8>(WeaponFlag)

UENUM(BlueprintType, Meta = (Bitflags))
enum class EWeaponFlags : uint8
{
	Firing     = (1 << 0),
	Reloading  = (1 << 1),
	END
};

UCLASS()
class TPSCPP_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	/**Gun mesh.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent* GunMesh;

	/**Muzzle. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USphereComponent* Muzzle;

public:	
	// Sets default values for this actor's properties
	AWeaponBase();

public:
	/**The maximum number of bullets in each magazine.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int AmmoMagazineMax = 30;
	/**The number of bullets in the current magazine.*/
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int AmmoMagazine = AmmoMagazineMax;
	/**The number of bullets the player can keep (including the ones in the gun).*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int AmmoMax = 300;
	/**The number of bullets in player's inventory (not in the gun).*/
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int AmmoInventory = AmmoMax - AmmoMagazine;

	/**The number of bullets fired in each shot.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int AmmoPerShot = 1;

	/**The angle between the forward vector (along which the bullet is supposed to travel) and the real vector (along which the bullet really travels).*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accuracy")
	float BulletSpreadAngleRad;

	/**How much the camera moves up. Must be negative to work properly.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accuracy")
	float RecoilScale;

	/**The recoil curve.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accuracy")
	class UCurveFloat* RecoilCurve;

	/**Recoil timeline*/
	FTimeline RecoilTimeline;

	/**Camera shake scale.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accuracy")
	float CameraShakeScale = 1.0f;

	/**Camera shake class.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accuracy")
	TSubclassOf<UCameraShake> CameraShakeClass = TSubclassOf<UCameraShake>();

	/**How long before next shot.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire Rate")
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire Rate")
	FTimerHandle FireTimerHandle;

	/**Firing sound.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundCue* SoundFire;

	/**Muzzle flash effect.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParticleSystem")
	class UParticleSystem* EffectMuzzleFlash;

	/**Impact effect.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ParticleSystem")
	class UParticleSystem* EffectImpact;

	/**Is this weapon firing?*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	bool bFiring = false;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Flags", meta = (Bitmask, BitmaskEnum = EWeaponFlags))
	uint8 WeaponFlags;

	/**Impact effect.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AnimMontageReload;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Timeline for gun recoil.
	UFUNCTION()
	void TimelineRecoil_Update();

public:
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void StartFiring();
	bool StartFiring_Validate();
	virtual void StartFiring_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ApplyRecoil();
	bool ApplyRecoil_Validate();
	void ApplyRecoil_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void StopFiring();
	bool StopFiring_Validate();
	virtual void StopFiring_Implementation();

	/**Fire a bullet.*/
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Fire();
	bool Fire_Validate();
	virtual void Fire_Implementation();

	/**Reduce ammo after a shot.*/
	UFUNCTION(BlueprintCallable)
	void ReduceAmmo();

	/**Check if the magazine in the gun runs out of ammo.*/
	UFUNCTION(BlueprintCallable)
	bool IsCurrentMagazineEmpty();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Reload();
	bool Reload_Validate();
	virtual void Reload_Implementation();

	/**Check if this weapon can reload.*/
	bool CanReload();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
	void PlayShootingSound();
	bool PlayShootingSound_Validate();
	void PlayShootingSound_Implementation();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
	void PlayEffect(UParticleSystem* ParticleEffect, const FTransform& SpawnTransform, USceneComponent* AttachToComponent = nullptr);
	bool PlayEffect_Validate(UParticleSystem* ParticleEffect, const FTransform& SpawnTransform, USceneComponent* AttachToComponent = nullptr);
	void PlayEffect_Implementation(UParticleSystem* ParticleEffect, const FTransform& SpawnTransform, USceneComponent* AttachToComponent = nullptr);
};
