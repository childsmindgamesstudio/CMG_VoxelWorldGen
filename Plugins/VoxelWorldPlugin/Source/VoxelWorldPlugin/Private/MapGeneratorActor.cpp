#include "MapGeneratorActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMapGeneratorActor::AMapGeneratorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set default values
	WorldSizeInChunks = FIntVector(10, 10, 10);
	ChunkSize = 32;
	VoxelSize = 100.0f;
	SkinThickness = 3;
	SkinDensity = 1.0f;
	AirDensity = -1.0f;
	SolidDensity = 1.0f;
	bEnableSolidSkin = true;

	// Create default scene component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

// Called when the game starts or when spawned
void AMapGeneratorActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Spawn the world generator if it doesn't exist
	if (!WorldGenerator)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		WorldGenerator = GetWorld()->SpawnActor<AVoxelWorldGenerator>(AVoxelWorldGenerator::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		
		if (WorldGenerator)
		{
			// Configure the world generator
			WorldGenerator->WorldConfig.ChunkSize = ChunkSize;
			WorldGenerator->WorldConfig.VoxelSize = VoxelSize;
			WorldGenerator->WorldConfig.bUseDensityFields = true;
		}
	}
}

// Called every frame
void AMapGeneratorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Generate the map
void AMapGeneratorActor::GenerateMap()
{
	if (!WorldGenerator)
	{
		UE_LOG(LogTemp, Warning, TEXT("WorldGenerator is null!"));
		return;
	}

	// Clear any existing map
	ClearMap();

	// Configure the world generator
	WorldGenerator->WorldConfig.ChunkSize = ChunkSize;
	WorldGenerator->WorldConfig.VoxelSize = VoxelSize;
	WorldGenerator->WorldConfig.bUseDensityFields = true;

	// Generate all chunks
	for (int32 X = 0; X < WorldSizeInChunks.X; X++)
	{
		for (int32 Y = 0; Y < WorldSizeInChunks.Y; Y++)
		{
			for (int32 Z = 0; Z < WorldSizeInChunks.Z; Z++)
			{
				FIntVector ChunkPos(X, Y, Z);
				WorldGenerator->CreateChunkAtPosition(ChunkPos);
				GenerateChunkDensityField(ChunkPos);
			}
		}
	}

	// Create the solid skin if enabled
	if (bEnableSolidSkin)
	{
		CreateSolidSkin();
	}

	// Update all chunk meshes
	WorldGenerator->UpdateChunkMeshes();

	UE_LOG(LogTemp, Log, TEXT("Map generated with size %d x %d x %d chunks"), WorldSizeInChunks.X, WorldSizeInChunks.Y, WorldSizeInChunks.Z);
}

// Clear the existing map
void AMapGeneratorActor::ClearMap()
{
	if (WorldGenerator && WorldGenerator->LoadedChunks.Num() > 0)
	{
		// Clear all loaded chunks
		for (auto& Pair : WorldGenerator->LoadedChunks)
		{
			if (Pair.Value)
			{
				Pair.Value->Destroy();
			}
		}
		WorldGenerator->LoadedChunks.Empty();
	}

	UE_LOG(LogTemp, Log, TEXT("Map cleared"));
}

// Create the solid skin around the world
void AMapGeneratorActor::CreateSolidSkin()
{
	if (!WorldGenerator)
		return;

	// Calculate world boundaries
	FVector WorldMin = FVector(0, 0, 0);
	FVector WorldMax = FVector(
		WorldSizeInChunks.X * ChunkSize * VoxelSize,
		WorldSizeInChunks.Y * ChunkSize * VoxelSize,
		WorldSizeInChunks.Z * ChunkSize * VoxelSize
	);

	// Create skin on X faces (min and max)
	for (int32 Y = 0; Y < WorldSizeInChunks.Y * ChunkSize; Y++)
	{
		for (int32 Z = 0; Z < WorldSizeInChunks.Z * ChunkSize; Z++)
		{
			// Min X face
			for (int32 T = 0; T < SkinThickness; T++)
			{
				FVector WorldPos(T * VoxelSize, Y * VoxelSize, Z * VoxelSize);
				SetDensityAtWorldPosition(WorldPos, SkinDensity);
			}

			// Max X face
			for (int32 T = 0; T < SkinThickness; T++)
			{
				FVector WorldPos((WorldSizeInChunks.X * ChunkSize - 1 - T) * VoxelSize, Y * VoxelSize, Z * VoxelSize);
				SetDensityAtWorldPosition(WorldPos, SkinDensity);
			}
		}
	}

	// Create skin on Y faces (min and max)
	for (int32 X = 0; X < WorldSizeInChunks.X * ChunkSize; X++)
	{
		for (int32 Z = 0; Z < WorldSizeInChunks.Z * ChunkSize; Z++)
		{
			// Min Y face
			for (int32 T = 0; T < SkinThickness; T++)
			{
				FVector WorldPos(X * VoxelSize, T * VoxelSize, Z * VoxelSize);
				SetDensityAtWorldPosition(WorldPos, SkinDensity);
			}

			// Max Y face
			for (int32 T = 0; T < SkinThickness; T++)
			{
				FVector WorldPos(X * VoxelSize, (WorldSizeInChunks.Y * ChunkSize - 1 - T) * VoxelSize, Z * VoxelSize);
				SetDensityAtWorldPosition(WorldPos, SkinDensity);
			}
		}
	}

	// Create skin on Z faces (min and max)
	for (int32 X = 0; X < WorldSizeInChunks.X * ChunkSize; X++)
	{
		for (int32 Y = 0; Y < WorldSizeInChunks.Y * ChunkSize; Y++)
		{
			// Min Z face
			for (int32 T = 0; T < SkinThickness; T++)
			{
				FVector WorldPos(X * VoxelSize, Y * VoxelSize, T * VoxelSize);
				SetDensityAtWorldPosition(WorldPos, SkinDensity);
			}

			// Max Z face
			for (int32 T = 0; T < SkinThickness; T++)
			{
				FVector WorldPos(X * VoxelSize, Y * VoxelSize, (WorldSizeInChunks.Z * ChunkSize - 1 - T) * VoxelSize);
				SetDensityAtWorldPosition(WorldPos, SkinDensity);
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Solid skin created with thickness %d"), SkinThickness);
}

// Fill the interior with solid voxels
void AMapGeneratorActor::FillInteriorWithSolid()
{
	if (!WorldGenerator)
		return;

	// Fill interior with solid density
	for (int32 X = SkinThickness; X < WorldSizeInChunks.X * ChunkSize - SkinThickness; X++)
	{
		for (int32 Y = SkinThickness; Y < WorldSizeInChunks.Y * ChunkSize - SkinThickness; Y++)
		{
			for (int32 Z = SkinThickness; Z < WorldSizeInChunks.Z * ChunkSize - SkinThickness; Z++)
			{
				FVector WorldPos(X * VoxelSize, Y * VoxelSize, Z * VoxelSize);
				SetDensityAtWorldPosition(WorldPos, SolidDensity);
			}
		}
	}
}

// Generate density field for a chunk
void AMapGeneratorActor::GenerateChunkDensityField(FIntVector ChunkPosition)
{
	// For now, we're just creating a solid interior with the skin handling the boundaries
	// In a more advanced implementation, this could generate procedural terrain
}

// Set density value at world position
void AMapGeneratorActor::SetDensityAtWorldPosition(FVector WorldPosition, float Density)
{
	if (!WorldGenerator)
		return;

	// Update the density in the appropriate chunk
	WorldGenerator->UpdateWorldDensity(WorldPosition, Density);
}

// Get density value at world position
float AMapGeneratorActor::GetDensityAtWorldPosition(FVector WorldPosition)
{
	if (!WorldGenerator)
		return -1.0f;

	// Sample the density at the world position
	return WorldGenerator->SampleWorldDensity(WorldPosition);
}