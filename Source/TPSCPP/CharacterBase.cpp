// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "WeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "CharacterAIController.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACharacterBase::ACharacterBase(): Super()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Capsule Component
	CharacterCapsuleComponent = GetCapsuleComponent();
	CharacterCapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CharacterCapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Overlap);

	// Skeletal Mesh
	CharacterMeshComponent = GetMesh();
	CharacterMeshComponent->RelativeLocation = FVector(0.0f, 0.0f, -90.0f);
	CharacterMeshComponent->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);

	// Spring arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(CharacterCapsuleComponent);
	SpringArm->RelativeLocation = FVector(0.0f, 0.0f, 60.0f);
	SpringArm->TargetArmLength = 100.0f;
	SpringArm->SocketOffset = FVector(0, 50.0f, 10.0f);
	SpringArm->bUsePawnControlRotation = true;

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	// Mesh visibility
	MeshVisibility = CreateDefaultSubobject<USphereComponent>(TEXT("MeshVisibility"));
	MeshVisibility->SetupAttachment(Camera);
	MeshVisibility->RelativeLocation = FVector(-6.85f, 0.0f, 0.0f);
	MeshVisibility->SetSphereRadius(12.0f);

	// Configure AI Perception Stimuli Source
	AIPerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AI_StimuliSource"));
	AIPerceptionStimuliSource->bAutoRegister = true;
	AIPerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
	AIPerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Hearing>());
	AIPerceptionStimuliSource->RegisterWithPerceptionSystem();

	// Set AI Controller class
	AIControllerClass = TSubclassOf<ACharacterAIController>();

	// Enable replication
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Bind overlap events to MeshVisibility.
	MeshVisibility->OnComponentBeginOverlap.AddDynamic(this, &ACharacterBase::OnMeshVisibilityBeginOverlap);
	MeshVisibility->OnComponentEndOverlap.AddDynamic(this, &ACharacterBase::OnMeshVisibilityEndOverlap);
	
	// Enable crouching
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterBase, HealthCurrent);
	DOREPLIFETIME(ACharacterBase, WeaponCurrent);

	DOREPLIFETIME(ACharacterBase, bJumpButtonDown);
	DOREPLIFETIME(ACharacterBase, bCrouchButtonDown);
	DOREPLIFETIME(ACharacterBase, ControlRotation);
}

void ACharacterBase::InputMoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACharacterBase::InputMoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ACharacterBase::InputStartJumping()
{
	Jump();
	ReplicateStartJumping();

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (IsValid(MovementComponent))
	{
		if (MovementComponent->Velocity.Size() > 0)
		{
			MovementComponent->JumpZVelocity = 400.0f;
		}
		else
		{
			MovementComponent->JumpZVelocity = 400.0f;
		}
	}
}

void ACharacterBase::InputStopJumping()
{
	StopJumping();
	ReplicateStopJumping();

	UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (IsValid(MovementComponent))
	{
		if (MovementComponent->Velocity.Size() > 0)
		{
			MovementComponent->JumpZVelocity = 400.0f;
		}
		else
		{
			MovementComponent->JumpZVelocity = 400.0f;
		}
	}
}

bool ACharacterBase::ReplicateStartJumping_Validate()
{
	return true;
}

void ACharacterBase::ReplicateStartJumping_Implementation()
{
	bJumpButtonDown = CanJump();
}

bool ACharacterBase::ReplicateStopJumping_Validate()
{
	return true;
}

void ACharacterBase::ReplicateStopJumping_Implementation()
{
	bJumpButtonDown = false;
}

void ACharacterBase::InputStartCrouching()
{
	Crouch();
	ReplicateStartCrouching();
}

void ACharacterBase::InputStopCrouching()
{
	UnCrouch();
	ReplicateStopCrouching();
}

bool ACharacterBase::ReplicateStartCrouching_Validate()
{
	return true;
}
void ACharacterBase::ReplicateStartCrouching_Implementation()
{
	bCrouchButtonDown = true;
}

bool ACharacterBase::ReplicateStopCrouching_Validate()
{
	return true;
}
void ACharacterBase::ReplicateStopCrouching_Implementation()
{
	bCrouchButtonDown = false;
}

void ACharacterBase::InputStartFiring()
{
	StartFiring();
}

void ACharacterBase::InputStopFiring()
{
	StopFiring();
}

bool ACharacterBase::StartFiring_Validate()
{
	return true;
}

void ACharacterBase::StartFiring_Implementation()
{
	if (IsValid(WeaponCurrent))
	{
		WeaponCurrent->StartFiring();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponCurrent=nullptr"));
	}
}

bool ACharacterBase::StopFiring_Validate()
{
	return true;
}

void ACharacterBase::StopFiring_Implementation()
{
	if (!IsValid(WeaponCurrent))
	{
		WeaponCurrent->StopFiring();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponCurrent=nullptr"));
	}
}

void ACharacterBase::ReplicateControlRotation()
{
	if (HasAuthority() || IsLocallyControlled())
	{
		AController* Controller = GetController();
		if (IsValid(Controller))
		{
			ControlRotation = Controller->GetControlRotation();
		}
	}
}

void ACharacterBase::OnMeshVisibilityBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp == CharacterCapsuleComponent)
	{
		CharacterMeshComponent->SetOwnerNoSee(true);
		CharacterMeshComponent->bCastHiddenShadow = true;
	}

	UE_LOG(LogTemp, Display, TEXT("Begin overlap"));
}

void ACharacterBase::OnMeshVisibilityEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp == CharacterCapsuleComponent)
	{
		CharacterMeshComponent->SetOwnerNoSee(false);
	}

	UE_LOG(LogTemp, Display, TEXT("End overlap"));
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ReplicateControlRotation();
}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::InputMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::InputMoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacterBase::InputStartJumping);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacterBase::InputStopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ACharacterBase::InputStartCrouching);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ACharacterBase::InputStopCrouching);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::InputStartFiring);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ACharacterBase::InputStopFiring);
}

bool ACharacterBase::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor) const
{
	static const FName NAME_AILineOfSight = FName(TEXT("TestPawnLineOfSight"));

	FHitResult HitResult;

	auto Sockets = GetMesh()->GetAllSocketNames();

	// Check if AI can detect character's bone.
	for (int i = 0; i < Sockets.Num(); i++)
	{
		FVector socketLocation = GetMesh()->GetSocketLocation(Sockets[i]);
		const bool bHitSocket = GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, socketLocation, ECollisionChannel::ECC_Visibility, FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));
		NumberOfLoSChecksPerformed++;

		if (bHitSocket == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
		{
			OutSeenLocation = socketLocation;
			OutSightStrength = 1;

			return true;
		}
	}

	// Check if AI can detect character's "middle point".
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ObserverLocation, GetActorLocation(), ECollisionChannel::ECC_Visibility, FCollisionQueryParams(NAME_AILineOfSight, true, IgnoreActor));
	NumberOfLoSChecksPerformed++;

	if (bHit == false || (HitResult.Actor.IsValid() && HitResult.Actor->IsOwnedBy(this)))
	{
		OutSeenLocation = GetActorLocation();
		OutSightStrength = 1;

		return true;
	}

	OutSightStrength = 0;
	return false;
}

