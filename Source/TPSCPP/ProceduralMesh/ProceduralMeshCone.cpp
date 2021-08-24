// Programmer: Khoi Ho


#include "ProceduralMeshCone.h"

void AProceduralMeshCone::GenerateMesh()
{
	// Fix the radius.
	Radius.X = UKismetMathLibrary::Abs(Radius.X);
	Radius.Y = 0.0f;
	Radius.Z = 0.0f;

	// Fix the height.
	Height.X = 0.0f;
	Height.Y = 0.0f;
	Height.Z = UKismetMathLibrary::Abs(Height.Z);

	// Fix the resolution.
	Resolution = Resolution < 3 ? 3 : Resolution;

	// Define vertices.
	TArray<FVector> UniqueVertices;
	UniqueVertices.Add(FVector(0.0f, 0.0f, 0.0f));      // Bottom middle point.
	UniqueVertices.Add(FVector(0.0f, 0.0f, Height.Z));  // Top point.

	float CentralAngleInDegrees = 360.0f / Resolution;
	for (int i = 0; i < Resolution; i++)
	{
		FVector2D SideVertex2D = FVector2D(Radius.X, 0.0f);
		SideVertex2D = UKismetMathLibrary::GetRotated2D(SideVertex2D, CentralAngleInDegrees * i);
		UniqueVertices.Add(FVector(SideVertex2D.X, SideVertex2D.Y, 0.0f)); // Side vertex.
	}

	// Define triangles.
	TArray<int> Triangles;
	for (int i = 0; i < Resolution; i++)
	{
		Triangles.Add(2 + ((i + 1) % Resolution)); Triangles.Add(0); Triangles.Add(i + 2);  // Side triangle.
		Triangles.Add(i + 2); Triangles.Add(1); Triangles.Add(2 + ((i + 1) % Resolution));  // Bottom triangle.
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
	for (int i = 1; i <= RealVertices.Num() / 6; i++)
	{
		// Side triangle.
		UVs.Add(FVector2D(0.0f, 0.0f));
		UVs.Add(FVector2D(0.5f, 1.0f));
		UVs.Add(FVector2D(1.0f, 0.0f));

		// Bottom triangle.
		UVs.Add(FVector2D(0.0f, 0.0f));
		UVs.Add(FVector2D(0.5f, 1.0f));
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
