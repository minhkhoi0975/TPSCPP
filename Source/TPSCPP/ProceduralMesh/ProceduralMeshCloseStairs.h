// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMesh/ProceduralMeshBase.h"
#include "ProceduralMeshCloseStairs.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API AProceduralMeshCloseStairs : public AProceduralMeshBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh", Meta = (MakeEditWidget = true))
	FVector Size = FVector(100.0f, 100.0f, 100.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh")
	int NumberOfSteps = 10;

protected:
	virtual void GenerateMesh() override;
};
