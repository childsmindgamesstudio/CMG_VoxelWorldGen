#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelChunk.h"
#include "VoxelWorldGenerator.generated.h"

USTRUCT()
struct FVoxelWorldConfig
{
	GENERATED_BODY()

	// Size of each chunk in voxels
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChunkSize;

	// Render distance in chunks
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RenderDistance;

	// Voxel size in world units
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float VoxelSize;

	// Iso surface value for marching cubes
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IsoSurfaceValue;

	// Noise parameters for procedural generation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NoiseScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NoiseOctaves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NoisePersistence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NoiseLacunarity;

	// Use density fields instead of discrete voxels
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseDensityFields;

	FVoxelWorldConfig()
		: ChunkSize(32)
		, RenderDistance(8)
		, VoxelSize(100.0f)
		, IsoSurfaceValue(0.0f)
		, NoiseScale(0.01f)
		, NoiseOctaves(4)
		, NoisePersistence(0.5f)
		, NoiseLacunarity(2.0f)
		, bUseDensityFields(true)
	{
	}
};

UCLASS()
class VOXELWORLDPLUGIN_API AVoxelWorldGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVoxelWorldGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// World configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVoxelWorldConfig WorldConfig;

	// Map of loaded chunks
	UPROPERTY()
	TMap<FIntVector, AVoxelChunk*> LoadedChunks;

	// Player reference for tracking position
	UPROPERTY()
	AActor* PlayerReference;

	// Generate world around player
	UFUNCTION(BlueprintCallable)
	void GenerateWorld();

	// Load chunks around player
	UFUNCTION(BlueprintCallable)
	void LoadChunksAroundPlayer();

	// Unload distant chunks
	UFUNCTION(BlueprintCallable)
	void UnloadDistantChunks();

	// Get chunk at position
	UFUNCTION(BlueprintCallable)
	AVoxelChunk* GetChunkAtPosition(FIntVector ChunkPosition);

	// Create chunk at position
	UFUNCTION(BlueprintCallable)
	AVoxelChunk* CreateChunkAtPosition(FIntVector ChunkPosition);

	// Get voxel at world position
	UFUNCTION(BlueprintCallable)
	EVoxelType GetVoxelAtWorldPosition(FVector WorldPosition);

	// Set voxel at world position
	UFUNCTION(BlueprintCallable)
	void SetVoxelAtWorldPosition(FVector WorldPosition, EVoxelType Type);

	// Generate procedural density at world position
	float GenerateProceduralDensity(FVector WorldPosition);

	// Convert world position to chunk coordinates
	FIntVector WorldToChunkPosition(FVector WorldPosition);

	// Convert world position to local voxel coordinates
	FIntVector WorldToLocalVoxelPosition(FVector WorldPosition);

	// Convert chunk position to world position
	FVector ChunkToWorldPosition(FIntVector ChunkPosition);

	// Get neighboring chunks
	TArray<AVoxelChunk*> GetNeighborChunks(FIntVector ChunkPosition);

	// Update chunk meshes
	void UpdateChunkMeshes();

	// Save world to file
	UFUNCTION(BlueprintCallable)
	void SaveWorld();

	// Load world from file
	UFUNCTION(BlueprintCallable)
	void LoadWorld();

	// Generate density field for chunk
	void GenerateChunkDensityField(AVoxelChunk* Chunk);

	// Sample density at world position
	float SampleWorldDensity(FVector WorldPosition);

	// Update density at world position
	void UpdateWorldDensity(FVector WorldPosition, float NewDensity);
};