// Programmer: Khoi Ho


#include "ProceduralMeshArch.h"

void AProceduralMeshArch::GenerateMesh()
{
	// Fix outter radius.
	OutterRadius.X = UKismetMathLibrary::Abs(OutterRadius.X);
	OutterRadius.Y = 0;
	OutterRadius.Z = 0;

	// Fix inner radius.
	InnerRadius.X = UKismetMathLibrary::Clamp(InnerRadius.X, 0.0f, OutterRadius.X);
	InnerRadius.Y = 0;
	InnerRadius.Z = 0;

	// Fix height.
	Height.X = 0;
	Height.Y = 0;
	Height.Z = UKismetMathLibrary::Abs(Height.Z);

	// Fix angle.
	Angle = UKismetMathLibrary::Clamp(Angle, 0.0f, 360.0f);

	// Fix resolution.
	Resolution = Resolution < 1 ? 1 : Resolution;

	// Define vertices.
	TArray<FVector> UniqueVertices;

	float CentralAngleInDegrees = 90.0f / Resolution;
	for (int i = 0; i <= Resolution; i++)
	{
		// Inner vertices.
		FVector2D InnerVertex2D = FVector2D(InnerRadius.X, 0.0f);
		InnerVertex2D = UKismetMathLibrary::GetRotated2D(InnerVertex2D, CentralAngleInDegrees * i);	
		UniqueVertices.Add(FVector(InnerVertex2D, 0.0f));
		UniqueVertices.Add(FVector(InnerVertex2D, Height.Z));

		// Outter vertices.
		FVector2D OutterVertex2D = FVector2D(OutterRadius.X, 0.0f);
		OutterVertex2D = UKismetMathLibrary::GetRotated2D(OutterVertex2D, CentralAngleInDegrees * i);
		UniqueVertices.Add(FVector(OutterVertex2D, 0.0f));
		UniqueVertices.Add(FVector(OutterVertex2D, Height.Z));
	}

	// Define triangles.
	TArray<int> Triangles;

	for (int i = 1; i <= Resolution; i++)
	{
		Triangles.Add(4 * i - 2); Triangles.Add(4 * i + 2); Triangles.Add(4 * i); Triangles.Add(4 * i - 2); Triangles.Add(4 * i); Triangles.Add(4 * i - 4);         // Bottom quad.
		Triangles.Add(4 * i - 3); Triangles.Add(4 * i + 1); Triangles.Add(4 * i + 3); Triangles.Add(4 * i - 3); Triangles.Add(4 * i + 3); Triangles.Add(4 * i - 1); // Top quad.
		Triangles.Add(4 * i); Triangles.Add(4 * i + 1); Triangles.Add(4 * i - 3); Triangles.Add(4 * i); Triangles.Add(4 * i - 3); Triangles.Add(4 * i - 4);         // Inner curve quad.
		Triangles.Add(4 * i - 2); Triangles.Add(4 * i - 1); Triangles.Add(4 * i + 3); Triangles.Add(4 * i - 2); Triangles.Add(4 * i + 3); Triangles.Add(4 * i + 2); // Outter curve quad.
	}

	Triangles.Add(4 * Resolution + 2); Triangles.Add(4 * Resolution + 3); Triangles.Add(4 * Resolution + 1); Triangles.Add(4 * Resolution + 2); Triangles.Add(4 * Resolution + 1); Triangles.Add(4 * Resolution); // Back quad.
	Triangles.Add(0); Triangles.Add(1); Triangles.Add(3); Triangles.Add(0); Triangles.Add(3); Triangles.Add(2); // Front quad.

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
		UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(0.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 0.0f)); // Bottom quad.
		UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(0.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 0.0f)); // Top quad.
		UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(0.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 0.0f)); // Inner curve quad.
		UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(0.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 0.0f)); // Outter curve quad.
	}
	UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(0.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 0.0f)); // Back quad.
	UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(0.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(0.0f, 0.0f)); UVs.Add(FVector2D(1.0f, 1.0f)); UVs.Add(FVector2D(1.0f, 0.0f)); // Front quad.

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
