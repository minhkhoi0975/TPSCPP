// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMesh/ProceduralMeshBase.h"
#include "ProceduralMeshCone.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API AProceduralMeshCone : public AProceduralMeshBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh", Meta = (MakeEditWidget = true))
	FVector Radius = FVector(100.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh", Meta = (MakeEditWidget = true))
	FVector Height = FVector(0.0f, 0.0f, 100.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	int Resolution = 20;

protected:
	virtual void GenerateMesh() override;
	
};
