#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelWorldGenerator.h"
#include "MapGeneratorActor.generated.h"

UCLASS()
class VOXELWORLDPLUGIN_API AMapGeneratorActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMapGeneratorActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Size of the world in chunks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation")
	FIntVector WorldSizeInChunks;

	// Size of each chunk in voxels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation")
	int32 ChunkSize;

	// Voxel size in world units
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation")
	float VoxelSize;

	// Thickness of the solid outer skin
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation|Skin", meta = (ClampMin = "1"))
	int32 SkinThickness;

	// Density value for solid voxels in the skin
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation|Skin")
	float SkinDensity;

	// Density value for air/empty voxels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation")
	float AirDensity;

	// Density value for solid interior voxels
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation")
	float SolidDensity;

	// Enable/disable the solid outer skin
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Generation|Skin")
	bool bEnableSolidSkin;

	// Reference to the voxel world generator
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Map Generation")
	AVoxelWorldGenerator* WorldGenerator;

	// Generate the map
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Map Generation")
	void GenerateMap();

	// Clear the existing map
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Map Generation")
	void ClearMap();

private:
	// Create the solid skin around the world
	void CreateSolidSkin();

	// Fill the interior with solid voxels
	void FillInteriorWithSolid();

	// Generate density field for a chunk
	void GenerateChunkDensityField(FIntVector ChunkPosition);

	// Set density value at world position
	void SetDensityAtWorldPosition(FVector WorldPosition, float Density);

	// Get density value at world position
	float GetDensityAtWorldPosition(FVector WorldPosition);
};