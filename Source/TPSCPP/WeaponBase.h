// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

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
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Ammo")
	int AmmoMagazine = AmmoMagazineMax;
	/**The number of bullets the player can keep (including the ones in the gun).*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int AmmoMax = 300;
	/**The number of bullets in player's inventory (not in the gun).*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Ammo")
	int AmmoInventory = AmmoMax - AmmoMagazineMax;

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void StartFiring();
	bool StartFiring_Validate();
	void StartFiring_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void StopFiring();
	bool StopFiring_Validate();
	void StopFiring_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Fire();
	bool Fire_Validate();
	void Fire_Implementation();

	UFUNCTION(BlueprintCallable)
	bool IsGunOutOfAmmo();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
	void PlayShootingSound();
	bool PlayShootingSound_Validate();
	void PlayShootingSound_Implementation();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
	void PlayEffect(UParticleSystem* ParticleEffect, const FTransform& SpawnTransform, USceneComponent* AttachToComponent = nullptr);
	bool PlayEffect_Validate(UParticleSystem* ParticleEffect, const FTransform& SpawnTransform, USceneComponent* AttachToComponent = nullptr);
	void PlayEffect_Implementation(UParticleSystem* ParticleEffect, const FTransform& SpawnTransform, USceneComponent* AttachToComponent = nullptr);
};
