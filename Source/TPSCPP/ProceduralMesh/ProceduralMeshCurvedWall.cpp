// Programmer: Khoi Ho


#include "ProceduralMeshCurvedWall.h"

void AProceduralMeshCurvedWall::GenerateMesh()
{
	// Fix the size.
	Size.X = UKismetMathLibrary::Abs(Size.X);
	Size.Y = UKismetMathLibrary::Abs(Size.Y);
	Size.Z = UKismetMathLibrary::Abs(Size.Z);

	// Fix the radius.
	Radius.X = UKismetMathLibrary::Abs(Radius.X);
	Radius.Y = 0;
	Radius.Z = 0;

	// Fix the resolution.
	Resolution = Resolution < 1 ? 1 : Resolution;

	// Define vertices.
	TArray<FVector> UniqueVertices;

	UniqueVertices.Add(FVector(Size.X, Size.Y, 0.0f));
	UniqueVertices.Add(FVector(Size.X, Size.Y, Size.Z));

	UniqueVertices.Add(FVector(Size.X, 0.0f, 0.0f));
	UniqueVertices.Add(FVector(Size.X, 0.0f, Size.Z));

	float CentralAngleInDegrees = 90.0f / Resolution;
	for (int i = 0; i <= Resolution; i++)
	{
		FVector2D CurveVertex2D = FVector2D(Radius.X, 0.0f);
		CurveVertex2D = UKismetMathLibrary::GetRotated2D(CurveVertex2D, CentralAngleInDegrees * i);

		UniqueVertices.Add(FVector(CurveVertex2D, 0.0f));
		UniqueVertices.Add(FVector(CurveVertex2D, Size.Z));
	}

	UniqueVertices.Add(FVector(0.0f, Size.Y, 0.0f));
	UniqueVertices.Add(FVector(0.0f, Size.Y, Size.Z));

	// Define triangles.
	TArray<int> Triangles;
	
	for (int i = 1; i <= Resolution + 2; i++)
	{
		Triangles.Add(2 * i + 2); Triangles.Add(2 * i); Triangles.Add(0);      // Bottom triangle.
		Triangles.Add(1); Triangles.Add(2 * i + 1); Triangles.Add(2 * i + 3);  // Top triangle.
		Triangles.Add(2 * i + 2); Triangles.Add(2 * i + 3); Triangles.Add(2 * i + 1); Triangles.Add(2 * i + 2); Triangles.Add(2 * i + 1); Triangles.Add(2 * i); // Curve quad.
	}

	Triangles.Add(2); Triangles.Add(3); Triangles.Add(1); Triangles.Add(2); Triangles.Add(1); Triangles.Add(0); // Right side.
	Triangles.Add(0); Triangles.Add(1); Triangles.Add(2 * Resolution + 7); Triangles.Add(0); Triangles.Add(2 * Resolution + 7); Triangles.Add(2 * Resolution + 6); // Back side.

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
	for (int i = 1; i <= Resolution + 1; i++)
	{
		UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(0.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 0.0f)); // Bottom triangle.
		UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(0.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 0.0f)); // Top triangle.
		UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(0.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 0.0f)); // Curve quad.
	}
	UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(0.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 0.0f)); // Right side.
	UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(0.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 0.0f)); // Back side.

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
