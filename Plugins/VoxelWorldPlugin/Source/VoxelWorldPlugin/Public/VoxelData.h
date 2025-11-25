#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "VoxelData.generated.h"

UENUM(BlueprintType)
enum class EVoxelType : uint8
{
	VT_Empty UMETA(DisplayName = "Empty"),
	VT_Solid UMETA(DisplayName = "Solid"),
	VT_Water UMETA(DisplayName = "Water"),
	VT_Lava UMETA(DisplayName = "Lava")
};

USTRUCT(BlueprintType)
struct FVoxelStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVoxelType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Hardness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer Tags;

	// Density value for smooth terrain
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Density;
};

UCLASS()
class VOXELWORLDPLUGIN_API UVoxel : public UObject
{
	GENERATED_BODY()

public:
	// Voxel map storing position to voxel data
	TMap<FVector, FVoxelStruct> VoxelMap;

	// Last position for reference
	FVector LastPosition;

	// Helper functions
	static FVoxelStruct MakeVoxelStruct(
		EVoxelType Type,
		int32 MaterialID,
		float Hardness,
		const FGameplayTagContainer& Tags);

	static FVoxelStruct GetDefaultVoxelStruct();

	// Voxel operations
	UVoxel* SetVoxel(FVector Position, EVoxelType Type);
	UVoxel* SetVoxelFull(
		FVector Position,
		EVoxelType Type,
		int32 MaterialID,
		float Hardness,
		const FGameplayTagContainer& Tags);

	EVoxelType GetVoxel(FVector Position);
	int32 GetVoxelMaterial(FVector Position);
	float GetVoxelHardness(FVector Position);
	float GetVoxelDensity(FVector Position);

	void SetVoxelMaterial(FVector Position, int32 MaterialID);
	void SetVoxelHardness(FVector Position, float Hardness);
	void SetVoxelDensity(FVector Position, float Density);
	void RemoveVoxel(FVector Position);
	bool IsVoxelOccupied(FVector Position);

	UVoxel* FindClosestVoxel(FVector Position);
	void ReplaceVoxelType(EVoxelType OldType, EVoxelType NewType);

	UVoxel* SplitVoxel(UVoxel* TargetVoxel, float SplitRatio);
	UVoxel* MergeVoxels(const TArray<UVoxel*>& Voxels);
	UVoxel* DuplicateVoxel(UVoxel* TargetVoxel, FVector NewPosition);

	// Region operations
	TArray<UVoxel*> GetVoxelsInRegion(FBox Region);
	void FillVoxelRegion(FBox Region, EVoxelType Type);
	void ClearVoxelRegion(FBox Region);

	// Utility operations
	FVector GetVoxelCenter(UVoxel* Voxel);
	float GetVoxelVolume(UVoxel* Voxel);
	TArray<UVoxel*> GetVoxelNeighborsByType(UVoxel* Voxel, EVoxelType Type);

	UVoxel* WorldToVoxel(FVector Position);
	FVector VoxelToWorld(UVoxel* Voxel);

	TArray<uint8> SerializeVoxelMesh(UVoxel* Voxel);
	UVoxel* DeserializeVoxelMesh(const TArray<uint8>& Data, FVector Position);

	// Density field operations
	void SetDensityField(const TArray<float>& DensityField, const FIntVector& FieldSize);
	TArray<float> GetDensityField() const;
	float SampleDensityField(const FIntVector& Position, const FIntVector& FieldSize) const;
	void UpdateDensityField(const FIntVector& Position, float Value, const FIntVector& FieldSize);
};