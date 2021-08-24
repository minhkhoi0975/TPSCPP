// Programmer: Khoi Ho

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMesh/ProceduralMeshBase.h"
#include "ProceduralMeshRamp.generated.h"

/**
 * 
 */
UCLASS()
class TPSCPP_API AProceduralMeshRamp : public AProceduralMeshBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Mesh", Meta = (MakeEditWidget = true))
		FVector Size = FVector(100.0f, 100.0f, 100.0f);

protected:
	virtual void GenerateMesh() override;
};
