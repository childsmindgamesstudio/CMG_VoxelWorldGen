#pragma once

#include "CoreMinimal.h"

/**
 * Static tables for Marching Cubes algorithm
 */
class VOXELWORLDPLUGIN_API FVoxelTables
{
public:
	// Edge table for determining which edges are intersected
	static const int edgeTable[256];

	// Triangle table for determining how to triangulate each case
	static const int triTable[256][16];

	// Constants for density field processing
	static const float DEFAULT_ISO_LEVEL;
	static const float MIN_DENSITY;
	static const float MAX_DENSITY;
};