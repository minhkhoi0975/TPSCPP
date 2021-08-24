// Programmer: Khoi Ho


#include "ProceduralMeshFloatingStairs.h"

void AProceduralMeshFloatingStairs::GenerateMesh()
{
	// Fix the number of steps
	if (NumberOfSteps <= 0)
	{
		NumberOfSteps = 1;
	}

	// Calculate step size.
	FVector StepSize(Size.X, Size.Y / NumberOfSteps, Size.Z / NumberOfSteps);

	// Define vertices.
	TArray<FVector> UniqueVertices;
	for (int i = 0; i < NumberOfSteps; i++)
	{
		// Front vertices.
		UniqueVertices.Add(FVector(0.0f      , StepSize.Y * i, StepSize.Z * i));
		UniqueVertices.Add(FVector(StepSize.X, StepSize.Y * i, StepSize.Z * i));
		UniqueVertices.Add(FVector(0.0f      , StepSize.Y * i, StepSize.Z * (i + 1)));
		UniqueVertices.Add(FVector(StepSize.X, StepSize.Y * i, StepSize.Z * (i + 1)));
		// Back vertices.
		UniqueVertices.Add(FVector(      0.0f, StepSize.Y * (i + 1), StepSize.Z * (i + 1)));
		UniqueVertices.Add(FVector(StepSize.X, StepSize.Y * (i + 1), StepSize.Z * (i + 1)));
		UniqueVertices.Add(FVector(      0.0f, StepSize.Y * (i + 1), StepSize.Z * i));
		UniqueVertices.Add(FVector(StepSize.X, StepSize.Y * (i + 1), StepSize.Z * i));
	}

	// Define triangles.
	TArray<int> Triangles;
	for (int i = 0; i < NumberOfSteps; i++)
	{
		Triangles.Add(8 * i);     Triangles.Add(8 * i + 2); Triangles.Add(8 * i + 3); Triangles.Add(8 * i)    ; Triangles.Add(8 * i + 3); Triangles.Add(8 * i + 1); // Front quad of a step.
		Triangles.Add(8 * i + 7); Triangles.Add(8 * i + 5); Triangles.Add(8 * i + 4); Triangles.Add(8 * i + 7); Triangles.Add(8 * i + 4); Triangles.Add(8 * i + 6); // Back quad of a step.
		Triangles.Add(8 * i + 2); Triangles.Add(8 * i + 4); Triangles.Add(8 * i + 5); Triangles.Add(8 * i + 2); Triangles.Add(8 * i + 5); Triangles.Add(8 * i + 3); // Top quad of a step.
		Triangles.Add(8 * i + 1); Triangles.Add(8 * i + 7); Triangles.Add(8 * i + 6); Triangles.Add(8 * i + 1); Triangles.Add(8 * i + 6); Triangles.Add(8 * i);     // Bottom quad of a step.
		Triangles.Add(8 * i + 6); Triangles.Add(8 * i + 4); Triangles.Add(8 * i + 2); Triangles.Add(8 * i + 6); Triangles.Add(8 * i + 2); Triangles.Add(8 * i);     // Right quad of a step.
		Triangles.Add(8 * i + 1); Triangles.Add(8 * i + 3); Triangles.Add(8 * i + 5); Triangles.Add(8 * i + 1); Triangles.Add(8 * i + 5); Triangles.Add(8 * i + 7); // Left quad of a step.
	}
	
	// Duplicate vertices.
	TArray<FVector> RealVertices;
	for (int i = 0; i < Triangles.Num(); i++)
	{
		RealVertices.Add(UniqueVertices[Triangles[i]]);
	}

	TArray<int> RealTriangles;
	for (int i = 0; i < RealVertices.Num(); i++)
	{
		RealTriangles.Add(i);
	}

	// Define UVs.
	TArray<FVector2D> UVs;
	for (int i = 1; i <= RealTriangles.Num() / 6; i++)
	{
		UVs.Add(FVector2D(0.0f, 0.0f));
		UVs.Add(FVector2D(0.0f, 1.0f));
		UVs.Add(FVector2D(1.0f, 1.0f));
		UVs.Add(FVector2D(0.0f, 0.0f));
		UVs.Add(FVector2D(1.0f, 1.0f));
		UVs.Add(FVector2D(1.0f, 0.0f));
	}

	// Define normals.
	TArray<FVector> Normals;
	for (int i = 0; i < RealVertices.Num() / 3; i++)
	{
		FVector Vertex1 = RealVertices[3 * i];
		FVector Vertex2 = RealVertices[3 * i + 1];
		FVector Vertex3 = RealVertices[3 * i + 2];

		FVector Normal = UKismetMathLibrary::Cross_VectorVector(Vertex2 - Vertex1, Vertex2 - Vertex3);
		Normal /= Normal.Size();

		for (int j = 0; j < 3; j++)
		{
			Normals.Add(Normal);
		}
	}

	// Define vertex colors.
	TArray<FLinearColor> VertexColors;
	for (int i = 0; i < RealVertices.Num(); i++)
	{
		VertexColors.Add(FLinearColor(0.0f, 0.0f, 0.5f, 1.0f));
	}

	// Create mesh.
	ProceduralMesh->CreateMeshSection_LinearColor(0, RealVertices, RealTriangles, Normals, UVs, VertexColors, TArray<FProcMeshTangent>(), true);

	// Set the material of the mesh.
	ProceduralMesh->SetMaterial(0, Material);

	// Enable collision data
	ProceduralMesh->ContainsPhysicsTriMeshData(true);
}
