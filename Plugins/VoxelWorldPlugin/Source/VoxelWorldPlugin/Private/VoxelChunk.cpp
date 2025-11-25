// This file will maintain and edit Voxel Chunks
#include "VoxelChunk.h"
#include "VoxelWorldGenerator.h"
#include "MarchingCubes.h"
#include "Components/ProceduralMeshComponent.h"
#include "Engine/World.h"

// Sets default values
AVoxelChunk::AVoxelChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create default scene component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Create procedural mesh component
	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProceduralMesh->SetupAttachment(RootComponent);

	// Create voxel data object
	VoxelData = NewObject<UVoxel>();
	
	// Initialize variables
	bIsGenerated = false;
	ChunkPosition = FIntVector::ZeroValue;
	WorldGenerator = nullptr;
}

// Called when the game starts or when spawned
void AVoxelChunk::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AVoxelChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Generate the chunk mesh using marching cubes
void AVoxelChunk::GenerateChunk()
{
	if (WorldGenerator && WorldGenerator->WorldConfig.bUseDensityFields)
	{
		CreateMeshFromDensityField();
	}
	else
	{
		CreateMeshFromVoxels();
	}
	
	bIsGenerated = true;
}

// Update the chunk mesh
void AVoxelChunk::UpdateChunk()
{
	GenerateChunk();
}

// Clear the chunk
void AVoxelChunk::ClearChunk()
{
	if (ProceduralMesh)
	{
		ProceduralMesh->ClearAllMeshSections();
	}
	
	MeshData.Vertices.Empty();
	MeshData.Triangles.Empty();
	MeshData.UVs.Empty();
	MeshData.Colors.Empty();
	
	bIsGenerated = false;
}

// Get voxel at local position
EVoxelType AVoxelChunk::GetVoxelAtLocalPosition(FIntVector LocalPosition)
{
	if (VoxelData)
	{
		return VoxelData->GetVoxel(FVector(LocalPosition.X, LocalPosition.Y, LocalPosition.Z));
	}
	return EVoxelType::VT_Empty;
}

// Set voxel at local position
void AVoxelChunk::SetVoxelAtLocalPosition(FIntVector LocalPosition, EVoxelType Type)
{
	if (VoxelData)
	{
		VoxelData->SetVoxel(FVector(LocalPosition.X, LocalPosition.Y, LocalPosition.Z), Type);
	}
}

// Create mesh from voxel data using marching cubes
void AVoxelChunk::CreateMeshFromVoxels()
{
	// Implementation for discrete voxel mesh generation
	// This would convert the VoxelData to a mesh using marching cubes
}

// Create mesh from density field using marching cubes
void AVoxelChunk::CreateMeshFromDensityField()
{
	if (DensityField.Num() == 0)
	{
		InitializeDensityField();
	}

	// Clear existing mesh data
	MeshData.Vertices.Empty();
	MeshData.Triangles.Empty();
	MeshData.UVs.Empty();
	MeshData.Colors.Empty();

	// Get world configuration
	FVoxelWorldConfig Config = WorldGenerator->WorldConfig;
	FVector ChunkWorldPosition = WorldGenerator->ChunkToWorldPosition(ChunkPosition);

	// Generate mesh using marching cubes algorithm
	UMarchingCubes::GenerateMeshFromField(
		DensityField,
		ChunkWorldPosition,
		FIntVector(Config.ChunkSize, Config.ChunkSize, Config.ChunkSize),
		Config.IsoSurfaceValue,
		MeshData.Vertices,
		MeshData.Triangles,
		MeshData.Colors // Using Colors array for normals temporarily
	);

	// Apply the generated mesh to the procedural mesh component
	ApplyMesh();
}

// Calculate density at position for marching cubes
float AVoxelChunk::CalculateDensity(FVector WorldPosition)
{
	// For now, we're using the density field directly
	// This method can be expanded to interpolate between voxel values
	return 0.0f;
}

// Get interpolated vertex position along edge
FVector AVoxelChunk::GetInterpolatedVertex(FVector Corner1, FVector Corner2, float Density1, float Density2)
{
	// Linear interpolation between two corner points based on their densities
	float t = (0.0f - Density1) / (Density2 - Density1);
	return Corner1 + t * (Corner2 - Corner1);
}

// Get voxel data for marching cubes grid cell
void AVoxelChunk::GetGridCellData(FIntVector CellPosition, float Values[8], FVector Positions[8])
{
	// Implementation for getting grid cell data
}

// Generate mesh for a single cell using marching cubes
void AVoxelChunk::GenerateCellMesh(FIntVector CellPosition)
{
	// Implementation for generating mesh for a single cell
}

// Apply mesh to procedural component
void AVoxelChunk::ApplyMesh()
{
	if (ProceduralMesh && MeshData.Vertices.Num() > 0)
	{
		ProceduralMesh->ClearAllMeshSections();
		ProceduralMesh->CreateMeshSection(0, MeshData.Vertices, MeshData.Triangles, MeshData.Colors, MeshData.UVs, TArray<FProcMeshTangent>(), true);
	}
}

// Regenerate mesh asynchronously
void AVoxelChunk::RegenerateMeshAsync()
{
	// Implementation for async mesh regeneration
}

// Initialize density field for this chunk
void AVoxelChunk::InitializeDensityField()
{
	if (!WorldGenerator)
		return;

	FVoxelWorldConfig Config = WorldGenerator->WorldConfig;
	int32 FieldSize = Config.ChunkSize * Config.ChunkSize * Config.ChunkSize;
	DensityField.Empty(FieldSize);
	DensityField.SetNumZeroed(FieldSize);

	// Initialize with default air density
	for (int32 i = 0; i < FieldSize; i++)
	{
		DensityField[i] = Config.IsoSurfaceValue - 1.0f; // Air density
	}
}

// Update density field at position
void AVoxelChunk::UpdateDensityField(FIntVector LocalPosition, float DensityValue)
{
	if (!WorldGenerator)
		return;

	FVoxelWorldConfig Config = WorldGenerator->WorldConfig;
	
	// Bounds checking
	if (LocalPosition.X < 0 || LocalPosition.X >= Config.ChunkSize ||
		LocalPosition.Y < 0 || LocalPosition.Y >= Config.ChunkSize ||
		LocalPosition.Z < 0 || LocalPosition.Z >= Config.ChunkSize)
	{
		return;
	}

	// Calculate index in the density field array
	int32 Index = LocalPosition.Z * Config.ChunkSize * Config.ChunkSize + 
				  LocalPosition.Y * Config.ChunkSize + 
				  LocalPosition.X;

	// Update the density value
	if (Index >= 0 && Index < DensityField.Num())
	{
		DensityField[Index] = DensityValue;
	}
}

// Get density value at local position
float AVoxelChunk::GetDensityAtLocalPosition(FIntVector LocalPosition)
{
	if (!WorldGenerator)
		return -1.0f;

	FVoxelWorldConfig Config = WorldGenerator->WorldConfig;
	
	// Bounds checking
	if (LocalPosition.X < 0 || LocalPosition.X >= Config.ChunkSize ||
		LocalPosition.Y < 0 || LocalPosition.Y >= Config.ChunkSize ||
		LocalPosition.Z < 0 || LocalPosition.Z >= Config.ChunkSize)
	{
		return Config.IsoSurfaceValue - 1.0f; // Air density
	}

	// Calculate index in the density field array
	int32 Index = LocalPosition.Z * Config.ChunkSize * Config.ChunkSize + 
				  LocalPosition.Y * Config.ChunkSize + 
				  LocalPosition.X;

	// Return the density value
	if (Index >= 0 && Index < DensityField.Num())
	{
		return DensityField[Index];
	}

	return Config.IsoSurfaceValue - 1.0f; // Air density
}