// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "WeaponBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "Animation/AnimBlueprint.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "CharacterAIController.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACharacterBase::ACharacterBase(): Super()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Capsule Component
	CharacterCapsuleComponent = GetCapsuleComponent();
	CharacterCapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CharacterCapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CharacterCapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);

	// Skeletal Mesh
	CharacterMeshComponent = GetMesh();
	CharacterMeshComponent->RelativeLocation = FVector(0.0f, 0.0f, -90.0f);
	CharacterMeshComponent->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);
	CharacterMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	CharacterMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	// Set mesh and animation for CharacterMeshComponent
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(TEXT("/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin"));
	if (CharacterMesh.Succeeded())
	{
		CharacterMeshComponent->SetSkeletalMesh(CharacterMesh.Object);

		static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBlueprint(TEXT("/Game/MyAssets/Characters/AnimationBlueprint/CharacterAnimationBlueprint"));
		if (AnimBlueprint.Succeeded())
		{
			CharacterMeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
			//CharacterMeshComponent->SetAnimInstanceClass(AnimBlueprint.Object->StaticClass());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Cannot find /Game/MyAssets/Characters/AnimationBlueprint/CharacterAnimationBlueprint"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin"));
	}

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
	bJumpButtonDown = CanJump();
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
	bJumpButtonDown = false;
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
	bCrouchButtonDown = true;
	ReplicateStartCrouching();
}

void ACharacterBase::InputStopCrouching()
{
	UnCrouch();
	bCrouchButtonDown = false;
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

bool ACharacterBase::ApplyRecoil_Validate(float Val)
{
	return true;
}

void ACharacterBase::ApplyRecoil_Implementation(float Val)
{
	AddControllerPitchInput(Val);
}

bool ACharacterBase::ReplicateAnimMontage_Validate(UAnimMontage* AnimMontage)
{
	return true;
}

void ACharacterBase::ReplicateAnimMontage_Implementation(UAnimMontage* AnimMontage)
{
	PlayAnimMontage(AnimMontage);
}

void ACharacterBase::InputStartFiring()
{
	StartFiring();

	if (IsValid(WeaponCurrent))
	{
		FString AmmoMagazineString = FString::FromInt(WeaponCurrent->AmmoMagazine);
		FString AmmoInventoryString = FString::FromInt(WeaponCurrent->AmmoInventory);
		UE_LOG(LogTemp, Display, TEXT("%s/%s"), *AmmoMagazineString, *AmmoInventoryString);
	}
}

void ACharacterBase::InputStopFiring()
{
	StopFiring();
}

void ACharacterBase::InputReload()
{
	Reload();
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

bool ACharacterBase::Reload_Validate()
{
	return true;
}

void ACharacterBase::Reload_Implementation()
{
	if (IsValid(WeaponCurrent))
	{
		WeaponCurrent->Reload();
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

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ACharacterBase::InputReload);
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

