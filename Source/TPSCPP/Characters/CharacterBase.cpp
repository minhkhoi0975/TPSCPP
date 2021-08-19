// Programmer: Khoi Ho


#include "CharacterBase.h"
#include "Weapons/WeaponBase.h"
#include "PickUps/PickUpWeapon.h"
#include "Interfaces/Interactable.h"
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
#include "AIController/CharacterAIController.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

// Sets default values
ACharacterBase::ACharacterBase(): Super()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Capsule Component
	CharacterCapsuleComponent = GetCapsuleComponent();
	CharacterCapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CharacterCapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Overlap);
	CharacterCapsuleComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Overlap);

	// Skeletal Mesh
	CharacterMeshComponent = GetMesh();
	CharacterMeshComponent->RelativeLocation = FVector(0.0f, 0.0f, -90.0f);
	CharacterMeshComponent->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);
	CharacterMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Overlap);
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

	// Movement component.
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->RotationRate = FRotator(0.0f, 180.0f, 0.0f);
	CharacterMovementComponent->bUseControllerDesiredRotation = true;

	// Disable this so that AI does not make sharp rotation.
	bUseControllerRotationYaw = false;

	// Configure AI Perception Stimuli Source
	AIPerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AI_StimuliSource"));
	AIPerceptionStimuliSource->bAutoRegister = true;
	AIPerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
	AIPerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Hearing>());
	AIPerceptionStimuliSource->RegisterWithPerceptionSystem();

	// Set AI Controller class
	AIControllerClass = TSubclassOf<ACharacterAIController>();

	// Weapon inventory.
	Inventory.SetNum(4);    // Each character can carry up to 4 weapons.

	// Enable replication
	SetReplicates(true);
}

EFactionAttitude ACharacterBase::GetAttitudeTowardsCharacter(const ACharacterBase* OtherCharacter) const
{
	if (IsValid(OtherCharacter))
	{
		if (this->Faction == OtherCharacter->Faction)
		{
			return EFactionAttitude::Ally;
		}
		else if (this->Faction == EFaction::Neutral || OtherCharacter->Faction == EFaction::Neutral)
		{
			return EFactionAttitude::Neutral;
		}
		else
		{
			return EFactionAttitude::Enemy;
		}
	}

	return EFactionAttitude::Neutral;
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
	DOREPLIFETIME(ACharacterBase, Inventory);
	DOREPLIFETIME(ACharacterBase, WeaponCurrentIndex);

	DOREPLIFETIME(ACharacterBase, bJumpButtonDown);
	DOREPLIFETIME(ACharacterBase, bCrouchButtonDown);
	DOREPLIFETIME(ACharacterBase, ControlRotation);

	DOREPLIFETIME(ACharacterBase, CharacterFlags);
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

bool ACharacterBase::ReplicateAnimMontagePlay_Validate(UAnimMontage* AnimMontage)
{
	return true;
}

void ACharacterBase::ReplicateAnimMontagePlay_Implementation(UAnimMontage* AnimMontage)
{
	PlayAnimMontage(AnimMontage);
}

bool ACharacterBase::ReplicateAnimMontageJumpToSection_Validate(FName SectionName, const UAnimMontage* AnimMontage)
{
	return true;
}

void ACharacterBase::ReplicateAnimMontageJumpToSection_Implementation(FName SectionName, const UAnimMontage* AnimMontage)
{
	if (AnimMontage)
	{
		CharacterMeshComponent->GetAnimInstance()->Montage_JumpToSection(SectionName, AnimMontage);
	}
}

void ACharacterBase::InputStartFiring()
{
	StartFiring();
}

void ACharacterBase::InputStopFiring()
{
	StopFiring();
}

void ACharacterBase::InputReload()
{
	Reload();
}

void ACharacterBase::InputSwitchWeapon1()
{
	SwitchWeapon(0);
	UE_LOG(LogTemp, Display, TEXT("Switch to weapon 1"));
}

void ACharacterBase::InputSwitchWeapon2()
{
	SwitchWeapon(1);
	UE_LOG(LogTemp, Display, TEXT("Switch to weapon 2"));
}

void ACharacterBase::InputSwitchWeapon3()
{
	SwitchWeapon(2);
	UE_LOG(LogTemp, Display, TEXT("Switch to weapon 3"));
}

void ACharacterBase::InputSwitchWeapon4()
{
	SwitchWeapon(3);
	UE_LOG(LogTemp, Display, TEXT("Switch to weapon 4"));
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
	if (IsValid(WeaponCurrent))
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

bool ACharacterBase::SwitchWeapon_Validate(int NewWeaponCurrentIndex)
{
	return true;
}

void ACharacterBase::SwitchWeapon_Implementation(int NewWeaponCurrentIndex)
{
	if (Inventory.IsValidIndex(NewWeaponCurrentIndex) 
		&& (NewWeaponCurrentIndex != WeaponCurrentIndex  || !IsValid(WeaponCurrent))
		&& !(CharacterFlags & GetCharacterFlag(ECharacterFlags::SwitchingWeapon))
		&& IsValid(AnimMontageWeaponSwitch))
	{
		SaveCurrentWeaponInfo();
		WeaponCurrentIndex = NewWeaponCurrentIndex;
		ReplicateAnimMontagePlay(AnimMontageWeaponSwitch);
	}
}

void ACharacterBase::SpawnWeapon(TSubclassOf<AWeaponBase> WeaponClass, int AmmoMagazine, int AmmoInventory)
{
	if (HasAuthority())
	{
		// Spawn weapon
		WeaponCurrent = GetWorld()->SpawnActorDeferred<AWeaponBase>(WeaponClass, GetActorTransform(), this, this);
		if (IsValid(WeaponCurrent))
		{
			WeaponCurrent->AmmoMagazine = (AmmoMagazine >= 0 && AmmoMagazine <= WeaponCurrent->AmmoMagazineMax) ? AmmoMagazine : WeaponCurrent->AmmoMagazineMax;
			WeaponCurrent->AmmoInventory = (AmmoInventory >= 0 && AmmoInventory <= (WeaponCurrent->AmmoMax - WeaponCurrent->AmmoMagazine)) ? AmmoInventory : WeaponCurrent->AmmoMax - WeaponCurrent->AmmoMagazine;
			WeaponCurrent->FinishSpawning(WeaponCurrent->GetActorTransform());

			// Attach weapon to character's hands.
			FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);
			WeaponCurrent->AttachToComponent(CharacterMeshComponent, AttachmentTransformRules, "gun_socket");
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Cannot spawn weapon"));
		}
	}
}

void ACharacterBase::SaveCurrentWeaponInfo()
{
	if (HasAuthority())
	{
		if (IsValid(WeaponCurrent) && Inventory.IsValidIndex(WeaponCurrentIndex))
		{
			Inventory[WeaponCurrentIndex].WeaponClass = WeaponCurrent->GetClass();
			Inventory[WeaponCurrentIndex].AmmoMagazine = WeaponCurrent->AmmoMagazine;
			Inventory[WeaponCurrentIndex].AmmoInventory = WeaponCurrent->AmmoInventory;
		}
	}
}



void ACharacterBase::InputInteract()
{
	Interact();
}

void ACharacterBase::InputDropWeapon()
{
	DropWeapon();
}

bool ACharacterBase::Interact_Validate()
{
	return true;
}

void ACharacterBase::Interact_Implementation()
{
	FVector StartLocation = Camera->GetComponentLocation();
	FVector EndLocation = StartLocation + Camera->GetForwardVector() * 300.0f;

	FCollisionQueryParams CollisionQuerryParams;
	CollisionQuerryParams.AddIgnoredActor(this);
	CollisionQuerryParams.AddIgnoredActor(WeaponCurrent);

	FHitResult HitResult;
	bool Hit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, CollisionQuerryParams);
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 5.0f);

	AActor* HitActor = HitResult.GetActor();

	if (Hit && HitActor)
	{
		IInteractable* InteractableActor = nullptr;

		// Check if the hit actor implements IInteractable.
		InteractableActor = Cast<IInteractable>(HitActor);
		if (InteractableActor)
		{
			InteractableActor->OnInteracted(this);
			return;
		}

		// Check if the owner of the hit actor implements IInteractable.
		AActor* HitActorOwner = HitActor->GetOwner();
		if (IsValid(HitActorOwner))
		{
			InteractableActor = Cast<IInteractable>(HitActorOwner);
			if (InteractableActor)
			{
				InteractableActor->OnInteracted(this);
				return;
			}
		}
	}
}

bool ACharacterBase::DropWeapon_Validate()
{
	return true;
}

void ACharacterBase::DropWeapon_Implementation()
{
	if (IsValid(WeaponCurrent))
	{
		// Remove information about WeaponCurrent in Inventory.
		Inventory[WeaponCurrentIndex].WeaponClass = nullptr;
		Inventory[WeaponCurrentIndex].AmmoInventory = 0;
		Inventory[WeaponCurrentIndex].AmmoMagazine = 0;

		// Spawn a pick-up.
		APickUpWeapon* PickUpWeapon = GetWorld()->SpawnActorDeferred<APickUpWeapon>(APickUpWeapon::StaticClass(), GetActorTransform());
		if (IsValid(PickUpWeapon))
		{
			PickUpWeapon->WeaponInstance.WeaponClass = WeaponCurrent->GetClass();
			PickUpWeapon->WeaponInstance.AmmoMagazine = WeaponCurrent->AmmoMagazine;
			PickUpWeapon->WeaponInstance.AmmoInventory = WeaponCurrent->AmmoInventory;
			PickUpWeapon->bWeaponMeshSimulatesPhysics = true;

			PickUpWeapon->FinishSpawning(PickUpWeapon->GetActorTransform());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Cannot spawn weapon pick-up."));
		}
		
		// Destroy the weapon.
		WeaponCurrent->Destroy();

		// Automatically switch weapon.
		for (int i = 0; i < Inventory.Num(); i++)
		{
			if (Inventory[i].WeaponClass)
			{
				SwitchWeapon(i);
				return;
			}
		}
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

	//UE_LOG(LogTemp, Display, TEXT("Begin overlap"));
}

void ACharacterBase::OnMeshVisibilityEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp == CharacterCapsuleComponent)
	{
		CharacterMeshComponent->SetOwnerNoSee(false);
	}

	//UE_LOG(LogTemp, Display, TEXT("End overlap"));
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

	PlayerInputComponent->BindAction("Weapon1", IE_Pressed, this, &ACharacterBase::InputSwitchWeapon1);
	PlayerInputComponent->BindAction("Weapon2", IE_Pressed, this, &ACharacterBase::InputSwitchWeapon2);
	PlayerInputComponent->BindAction("Weapon3", IE_Pressed, this, &ACharacterBase::InputSwitchWeapon3);
	PlayerInputComponent->BindAction("Weapon4", IE_Pressed, this, &ACharacterBase::InputSwitchWeapon4);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ACharacterBase::InputInteract);

	PlayerInputComponent->BindAction("DropWeapon", IE_Pressed, this, &ACharacterBase::InputDropWeapon);
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

