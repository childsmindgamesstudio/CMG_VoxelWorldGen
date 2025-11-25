#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelData.h"
#include "VoxelChunk.generated.h"

class AVoxelWorldGenerator;

USTRUCT()
struct FVoxelChunkData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FVector> Vertices;

	UPROPERTY()
	TArray<int32> Triangles;

	UPROPERTY()
	TArray<FVector2D> UVs;

	UPROPERTY()
	TArray<FColor> Colors;
};

UCLASS()
class VOXELWORLDPLUGIN_API AVoxelChunk : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVoxelChunk();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Chunk position in chunk coordinates
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FIntVector ChunkPosition;

	// Voxel data for this chunk
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UVoxel* VoxelData;

	// Procedural mesh component for rendering
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProceduralMeshComponent* ProceduralMesh;

	// Reference to world generator
	UPROPERTY()
	AVoxelWorldGenerator* WorldGenerator;

	// Chunk state
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsGenerated;

	// Density field for this chunk
	UPROPERTY()
	TArray<float> DensityField;

	// Mesh data for this chunk
	FVoxelChunkData MeshData;

	// Generate the chunk mesh using marching cubes
	UFUNCTION(BlueprintCallable)
	void GenerateChunk();

	// Update the chunk mesh
	UFUNCTION(BlueprintCallable)
	void UpdateChunk();

	// Clear the chunk
	UFUNCTION(BlueprintCallable)
	void ClearChunk();

	// Get voxel at local position
	UFUNCTION(BlueprintCallable)
	EVoxelType GetVoxelAtLocalPosition(FIntVector LocalPosition);

	// Set voxel at local position
	UFUNCTION(BlueprintCallable)
	void SetVoxelAtLocalPosition(FIntVector LocalPosition, EVoxelType Type);

	// Create mesh from voxel data using marching cubes
	void CreateMeshFromVoxels();

	// Create mesh from density field using marching cubes
	void CreateMeshFromDensityField();

	// Calculate density at position for marching cubes
	float CalculateDensity(FVector WorldPosition);

	// Get interpolated vertex position along edge
	FVector GetInterpolatedVertex(FVector Corner1, FVector Corner2, float Density1, float Density2);

	// Get voxel data for marching cubes grid cell
	void GetGridCellData(FIntVector CellPosition, float Values[8], FVector Positions[8]);

	// Generate mesh for a single cell using marching cubes
	void GenerateCellMesh(FIntVector CellPosition);

	// Apply mesh to procedural component
	void ApplyMesh();

	// Regenerate mesh asynchronously
	void RegenerateMeshAsync();

	// Initialize density field for this chunk
	void InitializeDensityField();

	// Update density field at position
	void UpdateDensityField(FIntVector LocalPosition, float DensityValue);

	// Get density value at local position
	float GetDensityAtLocalPosition(FIntVector LocalPosition);
};