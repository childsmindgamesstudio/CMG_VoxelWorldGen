// this file Holds all world Generation logic and functionaluty
#include "VoxelWorldGenerator.h"
#include "VoxelChunk.h"
#include "MarchingCubes.h"
#include "Engine/World.h"

// Sets default values
AVoxelWorldGenerator::AVoxelWorldGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create default scene component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Set default player reference to nullptr
	PlayerReference = nullptr;
}

// Called when the game starts or when spawned
void AVoxelWorldGenerator::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AVoxelWorldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Generate world around player
void AVoxelWorldGenerator::GenerateWorld()
{
	// Implementation will be added later
}

// Load chunks around player
void AVoxelWorldGenerator::LoadChunksAroundPlayer()
{
	// Implementation will be added later
}

// Unload distant chunks
void AVoxelWorldGenerator::UnloadDistantChunks()
{
	// Implementation will be added later
}

// Get chunk at position
AVoxelChunk* AVoxelWorldGenerator::GetChunkAtPosition(FIntVector ChunkPosition)
{
	if (LoadedChunks.Contains(ChunkPosition))
	{
		return LoadedChunks[ChunkPosition];
	}
	return nullptr;
}

// Create chunk at position
AVoxelChunk* AVoxelWorldGenerator::CreateChunkAtPosition(FIntVector ChunkPosition)
{
	if (LoadedChunks.Contains(ChunkPosition))
	{
		return LoadedChunks[ChunkPosition];
	}

	// Spawn a new chunk
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FVector WorldPosition = ChunkToWorldPosition(ChunkPosition);
	
	AVoxelChunk* NewChunk = GetWorld()->SpawnActor<AVoxelChunk>(
		AVoxelChunk::StaticClass(), 
		WorldPosition, 
		FRotator::ZeroRotator, 
		SpawnParams);

	if (NewChunk)
	{
		NewChunk->ChunkPosition = ChunkPosition;
		NewChunk->WorldGenerator = this;
		LoadedChunks.Add(ChunkPosition, NewChunk);
	}

	return NewChunk;
}

// Get voxel at world position
EVoxelType AVoxelWorldGenerator::GetVoxelAtWorldPosition(FVector WorldPosition)
{
	FIntVector ChunkPos = WorldToChunkPosition(WorldPosition);
	AVoxelChunk* Chunk = GetChunkAtPosition(ChunkPos);
	
	if (Chunk && Chunk->VoxelData)
	{
		FIntVector LocalPos = WorldToLocalVoxelPosition(WorldPosition);
		return Chunk->VoxelData->GetVoxel(FVector(LocalPos.X, LocalPos.Y, LocalPos.Z));
	}
	
	return EVoxelType::VT_Empty;
}

// Set voxel at world position
void AVoxelWorldGenerator::SetVoxelAtWorldPosition(FVector WorldPosition, EVoxelType Type)
{
	FIntVector ChunkPos = WorldToChunkPosition(WorldPosition);
	AVoxelChunk* Chunk = GetChunkAtPosition(ChunkPos);
	
	if (!Chunk)
	{
		Chunk = CreateChunkAtPosition(ChunkPos);
	}
	
	if (Chunk && Chunk->VoxelData)
	{
		FIntVector LocalPos = WorldToLocalVoxelPosition(WorldPosition);
		Chunk->VoxelData->SetVoxel(FVector(LocalPos.X, LocalPos.Y, LocalPos.Z), Type);
	}
}

// Generate procedural density at world position
float AVoxelWorldGenerator::GenerateProceduralDensity(FVector WorldPosition)
{
	// For now, return a simple density value
	// This can be expanded to include noise functions, heightmaps, etc.
	return -1.0f;
}

// Convert world position to chunk coordinates
FIntVector AVoxelWorldGenerator::WorldToChunkPosition(FVector WorldPosition)
{
	return FIntVector(
		FMath::FloorToInt(WorldPosition.X / (WorldConfig.ChunkSize * WorldConfig.VoxelSize)),
		FMath::FloorToInt(WorldPosition.Y / (WorldConfig.ChunkSize * WorldConfig.VoxelSize)),
		FMath::FloorToInt(WorldPosition.Z / (WorldConfig.ChunkSize * WorldConfig.VoxelSize))
	);
}

// Convert world position to local voxel coordinates
FIntVector AVoxelWorldGenerator::WorldToLocalVoxelPosition(FVector WorldPosition)
{
	FIntVector ChunkPos = WorldToChunkPosition(WorldPosition);
	FVector ChunkWorldPos = ChunkToWorldPosition(ChunkPos);
	
	return FIntVector(
		FMath::FloorToInt((WorldPosition.X - ChunkWorldPos.X) / WorldConfig.VoxelSize),
		FMath::FloorToInt((WorldPosition.Y - ChunkWorldPos.Y) / WorldConfig.VoxelSize),
		FMath::FloorToInt((WorldPosition.Z - ChunkWorldPos.Z) / WorldConfig.VoxelSize)
	);
}

// Convert chunk position to world position
FVector AVoxelWorldGenerator::ChunkToWorldPosition(FIntVector ChunkPosition)
{
	return FVector(
		ChunkPosition.X * WorldConfig.ChunkSize * WorldConfig.VoxelSize,
		ChunkPosition.Y * WorldConfig.ChunkSize * WorldConfig.VoxelSize,
		ChunkPosition.Z * WorldConfig.ChunkSize * WorldConfig.VoxelSize
	);
}

// Get neighboring chunks
TArray<AVoxelChunk*> AVoxelWorldGenerator::GetNeighborChunks(FIntVector ChunkPosition)
{
	TArray<AVoxelChunk*> Neighbors;
	
	// Check all adjacent chunks
	for (int32 X = -1; X <= 1; X++)
	{
		for (int32 Y = -1; Y <= 1; Y++)
		{
			for (int32 Z = -1; Z <= 1; Z++)
			{
				if (X == 0 && Y == 0 && Z == 0)
					continue;
					
				FIntVector NeighborPos(ChunkPosition.X + X, ChunkPosition.Y + Y, ChunkPosition.Z + Z);
				AVoxelChunk* Neighbor = GetChunkAtPosition(NeighborPos);
				
				if (Neighbor)
				{
					Neighbors.Add(Neighbor);
				}
			}
		}
	}
	
	return Neighbors;
}

// Update chunk meshes
void AVoxelWorldGenerator::UpdateChunkMeshes()
{
	for (auto& Pair : LoadedChunks)
	{
		if (Pair.Value)
		{
			Pair.Value->UpdateChunk();
		}
	}
}

// Save world to file
void AVoxelWorldGenerator::SaveWorld()
{
	// Implementation will be added later
}

// Load world from file
void AVoxelWorldGenerator::LoadWorld()
{
	// Implementation will be added later
}

// Generate density field for chunk
void AVoxelWorldGenerator::GenerateChunkDensityField(AVoxelChunk* Chunk)
{
	if (!Chunk)
		return;
		
	// Initialize the chunk's density field
	Chunk->InitializeDensityField();
}

// Sample density at world position
float AVoxelWorldGenerator::SampleWorldDensity(FVector WorldPosition)
{
	FIntVector ChunkPos = WorldToChunkPosition(WorldPosition);
	AVoxelChunk* Chunk = GetChunkAtPosition(ChunkPos);
	
	if (Chunk)
	{
		FIntVector LocalPos = WorldToLocalVoxelPosition(WorldPosition);
		return Chunk->GetDensityAtLocalPosition(LocalPos);
	}
	
	return WorldConfig.IsoSurfaceValue - 1.0f; // Default to air
}

// Update density at world position
void AVoxelWorldGenerator::UpdateWorldDensity(FVector WorldPosition, float NewDensity)
{
	FIntVector ChunkPos = WorldToChunkPosition(WorldPosition);
	AVoxelChunk* Chunk = GetChunkAtPosition(ChunkPos);
	
	if (!Chunk)
	{
		Chunk = CreateChunkAtPosition(ChunkPos);
	}
	
	if (Chunk)
	{
		FIntVector LocalPos = WorldToLocalVoxelPosition(WorldPosition);
		Chunk->UpdateDensityField(LocalPos, NewDensity);
	}
}