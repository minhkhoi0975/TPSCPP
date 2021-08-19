// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "Components/TimelineComponent.h"
#include "Animation/AnimMontage.h"
#include "Weapons/WeaponBase.h"
#include "CharacterBase.generated.h"

/**
 * Factions
 */
UENUM(BlueprintType)
enum class EFaction : uint8
{
	Neutral,
	Green,
	Yellow
};

/**
 * Attitude towards faction
 */
UENUM(BlueprintType)
enum class EFactionAttitude : uint8
{
	Enemy,
	Neutral,
	Ally
};

#define GetCharacterFlag(CharacterFlag) static_cast<uint8>(CharacterFlag)

/**
 * Character flags
 */
UENUM(BlueprintType, Meta = (Bitflags))
enum class ECharacterFlags : uint8
{
	Jumping         = (1 << 0),
	Crouching       = (1 << 1),
	Firing          = (1 << 2),
	SwitchingWeapon = (1 << 3),
	END
};

/**
 * Character class
 */
UCLASS(Blueprintable)
class TPSCPP_API ACharacterBase : public ACharacter, public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	/** The capsule of the character.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Mesh)
	class UCapsuleComponent* CharacterCapsuleComponent;

    /** The mesh of the character.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Mesh)
	class USkeletalMeshComponent* CharacterMeshComponent;

	/** Spring arm and camera.*/
	UPROPERTY(VisibleAnywhere, Category=Camera)
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category=Camera)
	class UCameraComponent* Camera;

	/**A collision sphere used for hiding the character mesh when the camera clips the collision capsule.*/
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USphereComponent* MeshVisibility;

	/** AI*/
	UPROPERTY(VisibleDefaultsOnly, Category = AI)
	class UAIPerceptionStimuliSourceComponent* AIPerceptionStimuliSource;


public:
	// Sets default values for this character's properties
	ACharacterBase();

public:	
	/** The faction this character belongs to.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
	EFaction Faction = EFaction::Neutral;

	/** Maximum amount of health this character can have.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthMax = 100;
	/** Current amount of health. If it goes below 0, this character dies.*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthCurrent = HealthMax;

	/** Current weapon the character is holding on its hand.*/
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	class AWeaponBase* WeaponCurrent;

	/** Character's inventory*/
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<FWeaponInstance> Inventory;

	/** The index of WeaponCurrent*/
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int WeaponCurrentIndex;

	/** Is the jump button down?*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "InputStatus")
	bool bJumpButtonDown = false;

	/** Is the crouch button down?*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "InputStatus")
	bool bCrouchButtonDown = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* AnimMontageWeaponSwitch;

	/** Animation status*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "AnimationStatus")
	FRotator ControlRotation;

	/** Flags*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "AnimationStatus", Meta = (Bitmask, BitmaskEnum = ECharacterFlags))
	uint8 CharacterFlags;

	/** 
	 *   Inputs 
	 */
protected:
	// Movements
	void InputMoveForward(float Value);
	void InputMoveRight(float Value);

	void InputStartJumping();
	void InputStopJumping();

	void InputStartCrouching(); 
	void InputStopCrouching();

	// Firing
	void InputStartFiring();
	void InputStopFiring();

	// Reload
	void InputReload();

	// Switch Weapon
	void InputSwitchWeapon1();
	void InputSwitchWeapon2();
	void InputSwitchWeapon3();
	void InputSwitchWeapon4();

	// Interact
	void InputInteract();

	// Drop weapon
	void InputDropWeapon();

public:
	UFUNCTION(Server, Reliable, WithValidation)
	void StartFiring();
	bool StartFiring_Validate();
	virtual void StartFiring_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void StopFiring();
	bool StopFiring_Validate();
	virtual void StopFiring_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Reload();
	bool Reload_Validate();
	virtual void Reload_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void SwitchWeapon(int NewWeaponCurrentIndex);
	bool SwitchWeapon_Validate(int NewWeaponCurrentIndex);
	void SwitchWeapon_Implementation(int NewWeaponCurrentIndex);

	/** Spawn a weapon and attach it to character's hands. If AmmoMagazine < 0, then AmmoMagazine = AmmoMagazineMax. If AmmoInventory < 0, then AmmoInventory = AmmoMax - AmmoMagazine.*/
	UFUNCTION(BlueprintCallable)
	void SpawnWeapon(TSubclassOf<AWeaponBase> WeaponClass, int AmmoMagazine = -1, int AmmoInventory = -1);

	/** Save the info of the current weapon in Inventory.*/
	UFUNCTION(BlueprintCallable)
	void SaveCurrentWeaponInfo();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Interact();
	bool Interact_Validate();
	void Interact_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void DropWeapon();
	bool DropWeapon_Validate();
	void DropWeapon_Implementation();


	/** 
	 *   Animation Replication 
	 */
public:
	void ReplicateControlRotation();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ReplicateStartJumping();
	bool ReplicateStartJumping_Validate();
	void ReplicateStartJumping_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ReplicateStopJumping();
	bool ReplicateStopJumping_Validate();
	void ReplicateStopJumping_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ReplicateStartCrouching();
	void ReplicateStartCrouching_Implementation();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ReplicateStopCrouching();
	bool ReplicateStopCrouching_Validate();
	void ReplicateStopCrouching_Implementation();

	UFUNCTION(BlueprintCallable, Client, Reliable, WithValidation)
	void ApplyRecoil(float Val);
	bool ApplyRecoil_Validate(float Val);
	void ApplyRecoil_Implementation(float Val);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
	void ReplicateAnimMontagePlay(UAnimMontage* AnimMontage);
	bool ReplicateAnimMontagePlay_Validate(UAnimMontage* AnimMontage);
	void ReplicateAnimMontagePlay_Implementation(UAnimMontage* AnimMontage);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
	void ReplicateAnimMontageJumpToSection(FName SectionName, const UAnimMontage* AnimMontage);
	bool ReplicateAnimMontageJumpToSection_Validate(FName SectionName, const UAnimMontage* AnimMontage);
	void ReplicateAnimMontageJumpToSection_Implementation(FName SectionName, const UAnimMontage* AnimMontage);

	// Mesh Visibility Overlap Events
protected:
	UFUNCTION()
	void OnMeshVisibilityBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnMeshVisibilityEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Get attitude towards a character.
public:	
	UFUNCTION(BlueprintCallable)
	EFactionAttitude GetAttitudeTowardsCharacter(const ACharacterBase* OtherCharacter) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Required for replication
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Override this function so that AI can see character's whole body instead of just "middle point".
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const;
};
