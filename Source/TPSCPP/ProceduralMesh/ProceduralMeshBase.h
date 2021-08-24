// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInterface.h"
#include "ProceduralMeshBase.generated.h"

UCLASS()
class TPSCPP_API AProceduralMeshBase : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UProceduralMeshComponent* ProceduralMesh;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	UMaterialInterface* Material;
	
public:	
	// Sets default values for this actor's properties
	AProceduralMeshBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void GenerateMesh();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
