#ifndef TERRAIN_GENERATOR_H
#define TERRAIN_GENERATOR_H
#include <vector>

class Grid;

// wczytywanie tekstur z pliku - zdefiniowane w main 
extern unsigned int loadTexture(char const* path, int repeat);

class TerrainGenerator
{


public:

	

	// opis algorytmu https://www.lighthouse3d.com/opengl/terrain/index.php?fault
	void CreateFaultFormation(Grid& grid, int iterations, float filter);

	// filtrowanie terenu wyjasnione tutaj https://youtu.be/z9YML6j5yDg?t=597
	void ApplyFIRFilter(Grid& grid, float filter);
	float FIRFilterSinglePoint(Grid& grid, int x, int z, float prevVal, float filter);


	// wczytanie tekstur i wyznaczenie poziomow wysokosci dla tekstur
	void LoadTextures(const char*,const char*,const char*,const char* , std::vector<float> levels, Grid& grid);


	// zwraca wyznaczony poziom wysokosci
	float GetTextureLevel(int level);


private:

	// tekstury
	std::vector<float> HeightLevels; 
	unsigned int terrainTextureLv0;
	unsigned int terrainTextureLv1;
	unsigned int terrainTextureLv2;
	unsigned int terrainTextureLv3;

	void CalculateTextureHeightLevels(float minHeight, float maxHeight, std::vector<float>& levels);

	friend class Grid;
};
#endif
