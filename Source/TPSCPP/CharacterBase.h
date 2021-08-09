// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "CharacterBase.generated.h"

UCLASS(Blueprintable)
class TPSCPP_API ACharacterBase : public ACharacter, public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	/** The capsule of the character.*/
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class UCapsuleComponent* CharacterCapsuleComponent;

    /** The mesh of the character.*/
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* CharacterMeshComponent;

	/** Spring arm and camera.*/
	UPROPERTY(VisibleAnywhere, Category=Camera)
	class USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category=Camera)
	class UCameraComponent* Camera;

	/** AI*/
	UPROPERTY(VisibleDefaultsOnly, Category = AI)
	class UAIPerceptionStimuliSourceComponent* AIPerceptionStimuliSource;


public:
	// Sets default values for this character's properties
	ACharacterBase();

public:	
	/** Health*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float HealthCurrent;

	/** Input status*/
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "InputStatus")
	bool bJumpButtonDown = false;

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
	void InputMoveForward(float Value);
	void InputMoveRight(float Value);

	void InputStartJumping();
	void InputStopJumping();

	UFUNCTION(Server, Reliable, WithValidation)
	void ReplicateStartJumping();
	void ReplicateStartJumping_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void ReplicateStopJumping();
	void ReplicateStopJumping_Implementation();

	void InputStartCrouching(); 
	void InputStopCrouching();

	UFUNCTION(Server, Reliable, WithValidation)
	void ReplicateStartCrouching();
	void ReplicateStartCrouching_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void ReplicateStopCrouching();
	void ReplicateStopCrouching_Implementation();

	// Animation
protected:
	void ReplicateControlRotation();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Override this function so that AI can see character's whole body instead of just "middle point".
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const;
};
