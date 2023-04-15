#include "TerrainGenerator.h"
#include "grid.h"
#include <algorithm>


void TerrainGenerator::CreateFaultFormation(Grid& grid, int iterations, float filter)
{
	
	// Calculate Fault Formation

	float deltaHeight = grid.MaxHeight - grid.MinHeight;

	// iterations of algorithm
	for (int iter = 0; iter < iterations; iter++)
	{
		float iterRatio = ((float)iter / (float)iterations);

		float height = grid.MaxHeight - iterRatio * deltaHeight;

		//get 2 random points on the grid
		GridPoint p1, p2;
		grid.Generate2RandGridPoints(p1, p2);

		//line between those points - LINE 1
		int dirX = p2.x - p1.x;
		int dirZ = p2.z - p1.z;





		for (int z = 0; z < grid.Width; z++)
		{
			for (int x = 0; x < grid.Depth; x++)
			{
				//line between actual point and random point 1 - LINE 2
				int dirX_in = x - p1.x;
				int dirZ_in = z - p1.z;

				//cross product of LINE 1 and LINE 2
				int cross = dirX_in * dirZ - dirX * dirZ_in;

				//alter vertices on one side of LINE 1 only (positive cross product)
				if (cross > 0)
				{
					grid.SetGridPoint(z, x, grid.GetGridPoint(z, x) + height);
				}


			}
		}


	}

	filter = std::clamp(filter, 0.0f, 1.0f);

	ApplyFIRFilter(grid,filter);
	grid.NormalizeGrid();

}

void TerrainGenerator::ApplyFIRFilter(Grid& grid, float filter)
{
	//left to right
	for (int z = 0; z < grid.Depth; z++)
	{
		float prevVal = grid.GetGridPoint(0,z);
		for (int x = 1; x < grid.Width; x++)
		{
			prevVal = FIRFilterSinglePoint(grid,x, z, prevVal, filter);
		}
	}

	//right to left
	for (int z = 0; z < grid.Depth; z++)
	{
		float prevVal = grid.GetGridPoint(grid.Width - 1,z);
		for (int x = grid.Width - 2; x >= 0; x--)
		{
			prevVal = FIRFilterSinglePoint(grid,x, z, prevVal, filter);
		}
	}


	//bottom to top
	for (int x = 0; x < grid.Width; x++)
	{
		float prevVal = grid.GetGridPoint(x, 0);
		for (int z = 1; z < grid.Depth; z++)
		{
			prevVal = FIRFilterSinglePoint(grid, x, z, prevVal, filter);
		}
	}

	//top to bottom
	for (int x = 0; x < grid.Width; x++)
	{
		float prevVal = grid.GetGridPoint(x, grid.Depth - 1);
		for (int z = grid.Depth - 2; z >= 0; z--)
		{
			prevVal = FIRFilterSinglePoint(grid, x, z, prevVal, filter);
		}
	}
}

float TerrainGenerator::FIRFilterSinglePoint(Grid& grid, int x, int z, float prevVal, float filter)
{
	float curVal = grid.GetGridPoint(x, z);
	float newVal = filter * prevVal + (1 - filter) * curVal;
	grid.SetGridPoint(x, z, newVal);
	return newVal;
}

void TerrainGenerator::LoadTextures(const char* path0, const char* path1, const char* path2, const char* path3, std::vector<float> levels, Grid& grid)
{
	
	HeightLevels.resize(levels.size());
	std::copy(levels.begin(), levels.end(), HeightLevels.begin());

	terrainTextureLv0 = loadTexture(path0, GL_MIRRORED_REPEAT);
	terrainTextureLv1 = loadTexture(path1, GL_MIRRORED_REPEAT);
	terrainTextureLv2 = loadTexture(path2, GL_MIRRORED_REPEAT);
	terrainTextureLv3 = loadTexture(path3, GL_MIRRORED_REPEAT);

	CalculateTextureHeightLevels(grid.GetMinHeight(),grid.GetMaxHeight(), HeightLevels);
}

void TerrainGenerator::CalculateTextureHeightLevels(float minHeight, float maxHeight, std::vector<float>& levels)
{
	float delta = (maxHeight - minHeight);

	for (int i = 0; i < levels.size(); i++)
	{
		levels[i] = minHeight + levels[i] * delta;
	}
}

float TerrainGenerator::GetTextureLevel(int level)
{
	assert(level >= 0);
	assert(level < HeightLevels.size());

	return HeightLevels[level];

}