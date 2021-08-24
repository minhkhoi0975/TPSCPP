// Programmer: Khoi Ho


#include "ProceduralMeshBase.h"
#include "ConstructorHelpers.h"

// Sets default values
AProceduralMeshBase::AProceduralMeshBase(): Super()
{
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	RootComponent = ProceduralMesh;
	ProceduralMesh->bUseAsyncCooking = true;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material(TEXT("/Game/MyAssets/ProceduralMesh/GridMaterial"));
	if (Material.Succeeded())
	{
		this->Material = Material.Object;
	}

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AProceduralMeshBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProceduralMeshBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	GenerateMesh();
}

void AProceduralMeshBase::GenerateMesh()
{
}

// Called every frame
void AProceduralMeshBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
