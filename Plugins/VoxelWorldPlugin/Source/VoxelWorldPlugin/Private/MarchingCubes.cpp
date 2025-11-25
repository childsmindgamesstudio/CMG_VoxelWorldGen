#include "MarchingCubes.h"
#include "Tables.h"
#include "VoxelData.h"
#include "Engine/World.h"

// Generate mesh from a 3D density field using marching cubes
void UMarchingCubes::GenerateMeshFromField(
	const TArray<float>& DensityField,
	const FVector& FieldOrigin,
	const FIntVector& FieldSize,
	float IsoLevel,
	TArray<FVector>& OutVertices,
	TArray<int32>& OutTriangles,
	TArray<FVector>& OutNormals)
{
	// Clear output arrays
	OutVertices.Empty();
	OutTriangles.Empty();
	OutNormals.Empty();

	// Process each cell in the grid
	for (int32 z = 0; z < FieldSize.Z - 1; z++)
	{
		for (int32 y = 0; y < FieldSize.Y - 1; y++)
		{
			for (int32 x = 0; x < FieldSize.X - 1; x++)
			{
				// Create a grid cell
				FGridCell cell;
				
				// Fill the cell with corner positions and density values
				for (int32 k = 0; k < 2; k++)
				{
					for (int32 j = 0; j < 2; j++)
					{
						for (int32 i = 0; i < 2; i++)
						{
							int32 index = (z + k) * FieldSize.Y * FieldSize.X + (y + j) * FieldSize.X + (x + i);
							int32 cornerIndex = k * 4 + j * 2 + i;
							
							cell.Positions[cornerIndex] = FieldOrigin + FVector(x + i, y + j, z + k);
							
							if (index < DensityField.Num())
							{
								cell.Values[cornerIndex] = DensityField[index];
							}
							else
							{
								cell.Values[cornerIndex] = IsoLevel - 1.0f; // Default to air
							}
						}
					}
				}
				
				// Polygonize this cell
				PolygonizeCell(cell, IsoLevel, OutVertices, OutTriangles);
			}
		}
	}
	
	// Calculate normals (simple implementation)
	OutNormals.Empty(OutVertices.Num());
	for (int32 i = 0; i < OutVertices.Num(); i++)
	{
		OutNormals.Add(FVector(0, 0, 1)); // Default normal
	}
}

// Polygonize a single grid cell using marching cubes
int32 UMarchingCubes::PolygonizeCell(
	const FGridCell& Cell,
	float IsoLevel,
	TArray<FVector>& Vertices,
	TArray<int32>& Triangles)
{
	int32 cubeIndex = 0;
	FVector vertList[12];
	
	// Determine the index into the edge table which tells us which vertices are inside of the surface
	if (Cell.Values[0] < IsoLevel) cubeIndex |= 1;
	if (Cell.Values[1] < IsoLevel) cubeIndex |= 2;
	if (Cell.Values[2] < IsoLevel) cubeIndex |= 4;
	if (Cell.Values[3] < IsoLevel) cubeIndex |= 8;
	if (Cell.Values[4] < IsoLevel) cubeIndex |= 16;
	if (Cell.Values[5] < IsoLevel) cubeIndex |= 32;
	if (Cell.Values[6] < IsoLevel) cubeIndex |= 64;
	if (Cell.Values[7] < IsoLevel) cubeIndex |= 128;
	
	// Cube is entirely in/out of the surface
	if (FVoxelTables::edgeTable[cubeIndex] == 0)
		return 0;
	
	// Find the vertices where the surface intersects the cube
	if (FVoxelTables::edgeTable[cubeIndex] & 1)
		vertList[0] = InterpolateVertex(Cell.Positions[0], Cell.Positions[1], Cell.Values[0], Cell.Values[1], IsoLevel);
	if (FVoxelTables::edgeTable[cubeIndex] & 2)
		vertList[1] = InterpolateVertex(Cell.Positions[1], Cell.Positions[2], Cell.Values[1], Cell.Values[2], IsoLevel);
	if (FVoxelTables::edgeTable[cubeIndex] & 4)
		vertList[2] = InterpolateVertex(Cell.Positions[2], Cell.Positions[3], Cell.Values[2], Cell.Values[3], IsoLevel);
	if (FVoxelTables::edgeTable[cubeIndex] & 8)
		vertList[3] = InterpolateVertex(Cell.Positions[3], Cell.Positions[0], Cell.Values[3], Cell.Values[0], IsoLevel);
	if (FVoxelTables::edgeTable[cubeIndex] & 16)
		vertList[4] = InterpolateVertex(Cell.Positions[4], Cell.Positions[5], Cell.Values[4], Cell.Values[5], IsoLevel);
	if (FVoxelTables::edgeTable[cubeIndex] & 32)
		vertList[5] = InterpolateVertex(Cell.Positions[5], Cell.Positions[6], Cell.Values[5], Cell.Values[6], IsoLevel);
	if (FVoxelTables::edgeTable[cubeIndex] & 64)
		vertList[6] = InterpolateVertex(Cell.Positions[6], Cell.Positions[7], Cell.Values[6], Cell.Values[7], IsoLevel);
	if (FVoxelTables::edgeTable[cubeIndex] & 128)
		vertList[7] = InterpolateVertex(Cell.Positions[7], Cell.Positions[4], Cell.Values[7], Cell.Values[4], IsoLevel);
	if (FVoxelTables::edgeTable[cubeIndex] & 256)
		vertList[8] = InterpolateVertex(Cell.Positions[0], Cell.Positions[4], Cell.Values[0], Cell.Values[4], IsoLevel);
	if (FVoxelTables::edgeTable[cubeIndex] & 512)
		vertList[9] = InterpolateVertex(Cell.Positions[1], Cell.Positions[5], Cell.Values[1], Cell.Values[5], IsoLevel);
	if (FVoxelTables::edgeTable[cubeIndex] & 1024)
		vertList[10] = InterpolateVertex(Cell.Positions[2], Cell.Positions[6], Cell.Values[2], Cell.Values[6], IsoLevel);
	if (FVoxelTables::edgeTable[cubeIndex] & 2048)
		vertList[11] = InterpolateVertex(Cell.Positions[3], Cell.Positions[7], Cell.Values[3], Cell.Values[7], IsoLevel);
	
	// Create the triangles
	int32 nTriang = 0;
	for (int32 i = 0; FVoxelTables::triTable[cubeIndex][i] != -1; i += 3)
	{
		int32 baseIndex = Vertices.Num();
		Vertices.Add(vertList[FVoxelTables::triTable[cubeIndex][i]]);
		Vertices.Add(vertList[FVoxelTables::triTable[cubeIndex][i + 1]]);
		Vertices.Add(vertList[FVoxelTables::triTable[cubeIndex][i + 2]]);
		
		Triangles.Add(baseIndex);
		Triangles.Add(baseIndex + 1);
		Triangles.Add(baseIndex + 2);
		
		nTriang++;
	}
	
	return nTriang;
}

// Interpolate vertex position along an edge
FVector UMarchingCubes::InterpolateVertex(const FVector& P1, const FVector& P2, float Val1, float Val2, float IsoLevel)
{
	float mu;
	FVector p;
	
	if (FMath::Abs(IsoLevel - Val1) < 0.00001f)
		return P1;
	if (FMath::Abs(IsoLevel - Val2) < 0.00001f)
		return P2;
	if (FMath::Abs(Val1 - Val2) < 0.00001f)
		return P1;
	
	mu = (IsoLevel - Val1) / (Val2 - Val1);
	p.X = P1.X + mu * (P2.X - P1.X);
	p.Y = P1.Y + mu * (P2.Y - P1.Y);
	p.Z = P1.Z + mu * (P2.Z - P1.Z);
	
	return p;
}

// Calculate normal at a vertex position
FVector UMarchingCubes::CalculateNormal(
	const TArray<float>& DensityField,
	const FVector& Position,
	const FVector& FieldOrigin,
	const FIntVector& FieldSize,
	float Delta)
{
	// Simple gradient calculation
	return FVector(0, 0, 1); // Placeholder
}

// Get density value at a world position from voxel data
float UMarchingCubes::GetDensityAtPosition(UVoxel* VoxelData, const FVector& WorldPosition, float VoxelSize)
{
	if (!VoxelData)
		return -1.0f;
		
	// Convert world position to voxel coordinates
	FVector VoxelPos = WorldPosition / VoxelSize;
	
	// Get the voxel at this position
	return VoxelData->GetVoxelDensity(VoxelPos);
}

// Create a grid cell from voxel data
FGridCell UMarchingCubes::CreateGridCell(
	UVoxel* VoxelData,
	const FIntVector& CellPosition,
	float VoxelSize,
	float IsoLevel)
{
	FGridCell cell;
	
	// Fill the cell with corner positions and density values
	for (int32 k = 0; k < 2; k++)
	{
		for (int32 j = 0; j < 2; j++)
		{
			for (int32 i = 0; i < 2; i++)
			{
				int32 cornerIndex = k * 4 + j * 2 + i;
				FVector worldPos = FVector(CellPosition.X + i, CellPosition.Y + j, CellPosition.Z + k) * VoxelSize;
				
				cell.Positions[cornerIndex] = worldPos;
				cell.Values[cornerIndex] = GetDensityAtPosition(VoxelData, worldPos, VoxelSize);
			}
		}
	}
	
	return cell;
}

// Sample density field at position
float UMarchingCubes::SampleDensityField(
	const TArray<float>& DensityField,
	const FIntVector& FieldSize,
	const FIntVector& Position)
{
	// Bounds checking
	if (Position.X < 0 || Position.X >= FieldSize.X ||
		Position.Y < 0 || Position.Y >= FieldSize.Y ||
		Position.Z < 0 || Position.Z >= FieldSize.Z)
	{
		return -1.0f; // Default to air density
	}
	
	// Calculate index
	int32 Index = Position.Z * FieldSize.Y * FieldSize.X + Position.Y * FieldSize.X + Position.X;
	
	if (Index >= 0 && Index < DensityField.Num())
	{
		return DensityField[Index];
	}
	
	return -1.0f; // Default to air density
}

// Generate density field from procedural noise
void UMarchingCubes::GenerateProceduralDensityField(
	TArray<float>& OutDensityField,
	const FIntVector& FieldSize,
	const FVector& FieldOrigin,
	float Scale,
	int32 Octaves,
	float Persistence,
	float Lacunarity)
{
	// Initialize the density field
	int32 TotalSize = FieldSize.X * FieldSize.Y * FieldSize.Z;
	OutDensityField.Empty(TotalSize);
	OutDensityField.SetNumZeroed(TotalSize);
	
	// For now, fill with a simple pattern
	// In a real implementation, this would use noise functions
	for (int32 z = 0; z < FieldSize.Z; z++)
	{
		for (int32 y = 0; y < FieldSize.Y; y++)
		{
			for (int32 x = 0; x < FieldSize.X; x++)
			{
				int32 Index = z * FieldSize.Y * FieldSize.X + y * FieldSize.X + x;
				FVector WorldPos = FieldOrigin + FVector(x, y, z);
				
				// Simple sphere shape for demonstration
				float Distance = FVector::Distance(WorldPos, FieldOrigin + FVector(FieldSize.X/2, FieldSize.Y/2, FieldSize.Z/2));
				float Radius = FMath::Min(FieldSize.X, FMath::Min(FieldSize.Y, FieldSize.Z)) / 2.0f;
				
				// Inside the sphere = solid, outside = air
				OutDensityField[Index] = (Distance < Radius) ? 1.0f : -1.0f;
			}
		}
	}
}