// Programmer: Khoi Ho


#include "ProceduralMeshCube.h"

void AProceduralMeshCube::GenerateMesh()
{
	// Fix the size.
	Size.X = UKismetMathLibrary::Abs(Size.X);
	Size.Y = UKismetMathLibrary::Abs(Size.Y);
	Size.Z = UKismetMathLibrary::Abs(Size.Z);

	// Define vertices.
	TArray<FVector> UniqueVertices;
	UniqueVertices.Add(FVector(0.0f, 0.0f, 0.0f));
	UniqueVertices.Add(FVector(Size.X, 0.0f, 0.0f));
	UniqueVertices.Add(FVector(Size.X, Size.Y, 0.0f));
	UniqueVertices.Add(FVector(0.0f, Size.Y, 0.0f));
	UniqueVertices.Add(FVector(0.0f, 0.0f, Size.Z));
	UniqueVertices.Add(FVector(Size.X, 0.0f, Size.Z));
	UniqueVertices.Add(FVector(Size.X, Size.Y, Size.Z));
	UniqueVertices.Add(FVector(0.0f, Size.Y, Size.Z));

	// Define triangles.
	TArray<int> Triangles;
	Triangles.Add(0); Triangles.Add(1); Triangles.Add(2); Triangles.Add(0); Triangles.Add(2); Triangles.Add(3); // Bottom quad.
	Triangles.Add(4); Triangles.Add(7); Triangles.Add(6); Triangles.Add(4); Triangles.Add(6); Triangles.Add(5); // Top quad.
	Triangles.Add(0); Triangles.Add(4); Triangles.Add(5); Triangles.Add(0); Triangles.Add(5); Triangles.Add(1); // Front quad.
	Triangles.Add(2); Triangles.Add(6); Triangles.Add(7); Triangles.Add(2); Triangles.Add(7); Triangles.Add(3); // Back quad.
	Triangles.Add(3); Triangles.Add(7); Triangles.Add(4); Triangles.Add(3); Triangles.Add(4); Triangles.Add(0); // Left quad.
	Triangles.Add(1); Triangles.Add(5); Triangles.Add(6); Triangles.Add(1); Triangles.Add(6); Triangles.Add(2); // Top quad.

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
	for (int i = 0; i < 6; i++)
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

