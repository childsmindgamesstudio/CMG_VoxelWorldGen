// This file maintains and edits the data associated with voxels.
#include "VoxelData.h"
#include "Tables.h"
#include "Engine/Engine.h"

// ============================================================================
// Internal Helpers (Placeholder)
// ============================================================================

FVoxelStruct UVoxel::MakeVoxelStruct(
    EVoxelType Type,
    int32 MaterialID,
    float Hardness,
    const FGameplayTagContainer& Tags)
{
    FVoxelStruct V;
    V.Type = Type;
    V.MaterialID = MaterialID;
    V.Hardness = Hardness;
    V.Tags = Tags;
    V.Density = (Type == EVoxelType::VT_Solid) ? 1.0f : -1.0f;
    return V;
}

FVoxelStruct UVoxel::GetDefaultVoxelStruct()
{
    FVoxelStruct V;
    V.Type = EVoxelType::VT_Solid;
    V.MaterialID = 0;
    V.Hardness = 1.0f;
    V.Density = 1.0f;
    return V;
}

// ============================================================================
// Voxel Operations
// ============================================================================

UVoxel* UVoxel::SetVoxel(FVector Position, EVoxelType Type)
{
    FVoxelStruct NewVoxel = GetDefaultVoxelStruct();
    NewVoxel.Type = Type;
    NewVoxel.Density = (Type == EVoxelType::VT_Solid) ? 1.0f : -1.0f;

    VoxelMap.Add(Position, NewVoxel);
    return this;
}

UVoxel* UVoxel::SetVoxelFull(
    FVector Position,
    EVoxelType Type,
    int32 MaterialID,
    float Hardness,
    const FGameplayTagContainer& Tags)
{
    FVoxelStruct NewVoxel = MakeVoxelStruct(Type, MaterialID, Hardness, Tags);
    VoxelMap.Add(Position, NewVoxel);
    return this;
}

EVoxelType UVoxel::GetVoxel(FVector Position)
{
    if (VoxelMap.Contains(Position))
    {
        return VoxelMap[Position].Type;
    }
    return EVoxelType::VT_Empty;
}

int32 UVoxel::GetVoxelMaterial(FVector Position)
{
    if (VoxelMap.Contains(Position))
    {
        return VoxelMap[Position].MaterialID;
    }
    return 0;
}

float UVoxel::GetVoxelHardness(FVector Position)
{
    if (VoxelMap.Contains(Position))
    {
        return VoxelMap[Position].Hardness;
    }
    return 0.0f;
}

float UVoxel::GetVoxelDensity(FVector Position)
{
    if (VoxelMap.Contains(Position))
    {
        return VoxelMap[Position].Density;
    }
    return -1.0f; // Air density
}

void UVoxel::SetVoxelMaterial(FVector Position, int32 MaterialID)
{
    if (VoxelMap.Contains(Position))
    {
        VoxelMap[Position].MaterialID = MaterialID;
    }
}

void UVoxel::SetVoxelHardness(FVector Position, float Hardness)
{
    if (VoxelMap.Contains(Position))
    {
        VoxelMap[Position].Hardness = Hardness;
    }
}

void UVoxel::SetVoxelDensity(FVector Position, float Density)
{
    if (VoxelMap.Contains(Position))
    {
        VoxelMap[Position].Density = Density;
    }
}

void UVoxel::RemoveVoxel(FVector Position)
{
    VoxelMap.Remove(Position);
}

bool UVoxel::IsVoxelOccupied(FVector Position)
{
    return VoxelMap.Contains(Position);
}

UVoxel* UVoxel::FindClosestVoxel(FVector Position)
{
    FVector BestKey = FVector::ZeroVector;
    float BestDistanceSq = TNumericLimits<float>::Max();

    for (const auto& Pair : VoxelMap)
    {
        float DistSq = FVector::DistSquared(Pair.Key, Position);
        if (DistSq < BestDistanceSq)
        {
            BestDistanceSq = DistSq;
            BestKey = Pair.Key;
        }
    }

    if (BestDistanceSq < TNumericLimits<float>::Max())
    {
        return this;
    }

    return this;
}

void UVoxel::ReplaceVoxelType(EVoxelType OldType, EVoxelType NewType)
{
    for (auto& Pair : VoxelMap)
    {
        if (Pair.Value.Type == OldType)
        {
            Pair.Value.Type = NewType;
            Pair.Value.Density = (NewType == EVoxelType::VT_Solid) ? 1.0f : -1.0f;
        }
    }
}

UVoxel* UVoxel::SplitVoxel(UVoxel* TargetVoxel, float SplitRatio)
{
    // Placeholder logic
    return this;
}

UVoxel* UVoxel::MergeVoxels(const TArray<UVoxel*>& Voxels)
{
    // Placeholder logic
    return this;
}

UVoxel* UVoxel::DuplicateVoxel(UVoxel* TargetVoxel, FVector NewPosition)
{
    if (!TargetVoxel) return this;

    if (VoxelMap.Contains(TargetVoxel->LastPosition))
    {
        FVoxelStruct Copy = VoxelMap[TargetVoxel->LastPosition];
        VoxelMap.Add(NewPosition, Copy);
    }
    return this;
}

// ============================================================================
// Region Operations
// ============================================================================

TArray<UVoxel*> UVoxel::GetVoxelsInRegion(FBox Region)
{
    TArray<UVoxel*> Out;

    for (const auto& Pair : VoxelMap)
    {
        if (Region.IsInside(Pair.Key))
        {
            Out.Add(this);
        }
    }

    return Out;
}

void UVoxel::FillVoxelRegion(FBox Region, EVoxelType Type)
{
    for (float X = Region.Min.X; X <= Region.Max.X; X++)
    for (float Y = Region.Min.Y; Y <= Region.Max.Y; Y++)
    for (float Z = Region.Min.Z; Z <= Region.Max.Z; Z++)
    {
        SetVoxel(FVector(X, Y, Z), Type);
    }
}

void UVoxel::ClearVoxelRegion(FBox Region)
{
    TArray<FVector> ToRemove;

    for (const auto& Pair : VoxelMap)
    {
        if (Region.IsInside(Pair.Key))
        {
            ToRemove.Add(Pair.Key);
        }
    }

    for (const FVector& Key : ToRemove)
    {
        VoxelMap.Remove(Key);
    }
}

// ============================================================================
// Utility Operations
// ============================================================================

FVector UVoxel::GetVoxelCenter(UVoxel* Voxel)
{
    return FVector::ZeroVector;
}

float UVoxel::GetVoxelVolume(UVoxel* Voxel)
{
    return 1.0f;
}

TArray<UVoxel*> UVoxel::GetVoxelNeighborsByType(UVoxel* Voxel, EVoxelType Type)
{
    return TArray<UVoxel*>();
}

UVoxel* UVoxel::WorldToVoxel(FVector Position)
{
    return this;
}

FVector UVoxel::VoxelToWorld(UVoxel* Voxel)
{
    return FVector::ZeroVector;
}

TArray<uint8> UVoxel::SerializeVoxelMesh(UVoxel* Voxel)
{
    return TArray<uint8>();
}

UVoxel* UVoxel::DeserializeVoxelMesh(const TArray<uint8>& Data, FVector Position)
{
    return this;
}

// ============================================================================
// Density Field Operations
// ============================================================================

void UVoxel::SetDensityField(const TArray<float>& DensityField, const FIntVector& FieldSize)
{
    // Convert density field to voxel data
    int32 Index = 0;
    for (int32 z = 0; z < FieldSize.Z; z++)
    {
        for (int32 y = 0; y < FieldSize.Y; y++)
        {
            for (int32 x = 0; x < FieldSize.X; x++)
            {
                if (Index < DensityField.Num())
                {
                    FVector Position(x, y, z);
                    FVoxelStruct Voxel = GetDefaultVoxelStruct();
                    Voxel.Density = DensityField[Index];
                    Voxel.Type = (Voxel.Density > 0.0f) ? EVoxelType::VT_Solid : EVoxelType::VT_Empty;
                    VoxelMap.Add(Position, Voxel);
                }
                Index++;
            }
        }
    }
}

TArray<float> UVoxel::GetDensityField() const
{
    TArray<float> DensityField;
    // Extract density values from voxel map
    for (const auto& Pair : VoxelMap)
    {
        DensityField.Add(Pair.Value.Density);
    }
    return DensityField;
}

float UVoxel::SampleDensityField(const FIntVector& Position, const FIntVector& FieldSize) const
{
    FVector VoxelPos(Position.X, Position.Y, Position.Z);
    if (VoxelMap.Contains(VoxelPos))
    {
        return VoxelMap[VoxelPos].Density;
    }
    return -1.0f; // Air density
}

void UVoxel::UpdateDensityField(const FIntVector& Position, float Value, const FIntVector& FieldSize)
{
    FVector VoxelPos(Position.X, Position.Y, Position.Z);
    if (VoxelMap.Contains(VoxelPos))
    {
        VoxelMap[VoxelPos].Density = Value;
        VoxelMap[VoxelPos].Type = (Value > 0.0f) ? EVoxelType::VT_Solid : EVoxelType::VT_Empty;
    }
    else
    {
        FVoxelStruct Voxel = GetDefaultVoxelStruct();
        Voxel.Density = Value;
        Voxel.Type = (Value > 0.0f) ? EVoxelType::VT_Solid : EVoxelType::VT_Empty;
        VoxelMap.Add(VoxelPos, Voxel);
    }
}