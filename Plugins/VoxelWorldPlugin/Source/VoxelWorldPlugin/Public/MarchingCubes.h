#pragma once

#include "CoreMinimal.h"
#include "VoxelData.h"
#include "MarchingCubes.generated.h"

USTRUCT()
struct FMCVertex
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Position;

	UPROPERTY()
	FVector Normal;

	FMCVertex()
		: Position(FVector::ZeroVector)
		, Normal(FVector::ZeroVector)
	{
	}

	FMCVertex(const FVector& InPosition)
		: Position(InPosition)
		, Normal(FVector::ZeroVector)
	{
	}

	FMCVertex(const FVector& InPosition, const FVector& InNormal)
		: Position(InPosition)
		, Normal(InNormal)
	{
	}
};

USTRUCT()
struct FMCTriangle
{
	GENERATED_BODY()

	UPROPERTY()
	FMCVertex Vertex1;

	UPROPERTY()
	FMCVertex Vertex2;

	UPROPERTY()
	FMCVertex Vertex3;

	FMCTriangle()
	{
	}

	FMCTriangle(const FMCVertex& V1, const FMCVertex& V2, const FMCVertex& V3)
		: Vertex1(V1)
		, Vertex2(V2)
		, Vertex3(V3)
	{
	}
};

/**
 * Class implementing the Marching Cubes algorithm for smooth voxel terrain generation
 */
UCLASS()
class VOXELWORLDPLUGIN_API UMarchingCubes : public UObject
{
	GENERATED_BODY()

public:
	// Struct representing a grid cell for marching cubes
	struct FGridCell
	{
		FVector Positions[8];  // Corner positions
		float Values[8];       // Density values at corners
	};

	// Generate mesh from a 3D density field using marching cubes
	static void GenerateMeshFromField(
		const TArray<float>& DensityField,
		const FVector& FieldOrigin,
		const FIntVector& FieldSize,
		float IsoLevel,
		TArray<FVector>& OutVertices,
		TArray<int32>& OutTriangles,
		TArray<FVector>& OutNormals);

	// Polygonize a single grid cell using marching cubes
	static int32 PolygonizeCell(
		const FGridCell& Cell,
		float IsoLevel,
		TArray<FVector>& Vertices,
		TArray<int32>& Triangles);

	// Interpolate vertex position along an edge
	static FVector InterpolateVertex(const FVector& P1, const FVector& P2, float Val1, float Val2, float IsoLevel);

	// Calculate normal at a vertex position
	static FVector CalculateNormal(
		const TArray<float>& DensityField,
		const FVector& Position,
		const FVector& FieldOrigin,
		const FIntVector& FieldSize,
		float Delta);

	// Get density value at a world position from voxel data
	static float GetDensityAtPosition(UVoxel* VoxelData, const FVector& WorldPosition, float VoxelSize);

	// Create a grid cell from voxel data
	static FGridCell CreateGridCell(
		UVoxel* VoxelData,
		const FIntVector& CellPosition,
		float VoxelSize,
		float IsoLevel);

	// Sample density field at position
	static float SampleDensityField(
		const TArray<float>& DensityField,
		const FIntVector& FieldSize,
		const FIntVector& Position);

	// Generate density field from procedural noise
	static void GenerateProceduralDensityField(
		TArray<float>& OutDensityField,
		const FIntVector& FieldSize,
		const FVector& FieldOrigin,
		float Scale,
		int32 Octaves,
		float Persistence,
		float Lacunarity);
};