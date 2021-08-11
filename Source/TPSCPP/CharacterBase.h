// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "Components/TimelineComponent.h"
#include "CharacterBase.generated.h"

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
	/** Maximum amount of health this character can have.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthMax = 100;
	/** Current amount of health. If it goes below 0, this character dies.*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthCurrent = HealthMax;

	/** Current weapon the character is holding on its hand.*/
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	class AWeaponBase* WeaponCurrent;

	/** Is the jump button down?*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "InputStatus")
	bool bJumpButtonDown = false;

	/** Is the crouch button down?*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "InputStatus")
	bool bCrouchButtonDown = false;

	/** Animation status*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "AnimationStatus")
	FRotator ControlRotation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Required for replication
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Inputs
protected:

	// Movements

	void InputMoveForward(float Value);
	void InputMoveRight(float Value);

	void InputStartJumping();
	void InputStopJumping();

	UFUNCTION(Server, Reliable, WithValidation)
	void ReplicateStartJumping();
	bool ReplicateStartJumping_Validate();
	void ReplicateStartJumping_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void ReplicateStopJumping();
	bool ReplicateStopJumping_Validate();
	void ReplicateStopJumping_Implementation();

	void InputStartCrouching(); 
	void InputStopCrouching();

	UFUNCTION(Server, Reliable, WithValidation)
	void ReplicateStartCrouching();
	void ReplicateStartCrouching_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void ReplicateStopCrouching();
	bool ReplicateStopCrouching_Validate();
	void ReplicateStopCrouching_Implementation();

	// Firing

	void InputStartFiring();
	void InputStopFiring();

	UFUNCTION(Server, Reliable, WithValidation)
	void StartFiring();
	bool StartFiring_Validate();
	void StartFiring_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void StopFiring();
	bool StopFiring_Validate();
	void StopFiring_Implementation();


	// Animation
protected:
	void ReplicateControlRotation();

	// Mesh Visibility Overlap Events
	UFUNCTION()
	void OnMeshVisibilityBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnMeshVisibilityEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Override this function so that AI can see character's whole body instead of just "middle point".
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const;
};
