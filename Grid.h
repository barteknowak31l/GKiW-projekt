#ifndef GRID_H
#define GRID_H

#include "TerrainGenerator.h"
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader;

// opisuje punkt na siatce
struct GridPoint {
	int x = 0;
	int z = 0;

	bool IsEqual(GridPoint& p) const {
		return ((x == p.x) && (z == p.z));
	}

	void Print()
	{
		std::cout << x << " " << z << "\n";
	}
};

// opisuje wierzcholki siatki // dane do shadera
struct GridVertex {
	glm::vec3 Pos;
	glm::vec2 Tex;
	glm::vec3 Normal = glm::vec3(0.0f, 0.0f, 0.0f);
	void InitVertex(int x, int y, int z, int Width, int Depth, float worldScale);
};

// definiuje siatkê terenu o wymiarach [ Width X Depth ] oraz wysokoœci dla ka¿dego punktu danej wartoœci¹ tablicy pGrid[Width][Depth] 
class Grid {

public:

	// wymiary grida
	int Width, Depth;
	
	// zakres wysokoœci
	float MinHeight, MaxHeight;

	// do przeskalowania wspolrzednej Y
	float HeightScaleFactor;

	// sluzy do przeskalowania wspolrzednych X , Z
	float WorldScale;
	

	Grid(int W, int D, float WS,float heightScaleFactor , float MinH, float MaxH, int iter, float filter);
	~Grid();


	// get/set wartosci z tablicy pGrid
	float GetGridPoint(int x, int z);
	void  SetGridPoint(int x, int z, float value);


	// generuje 2 (rozne) losowe punkty na siatce 
	void Generate2RandGridPoints(GridPoint& p1, GridPoint& p2);


	// funkcja rysujaca teren
	void Draw(Shader& shader);

	// normalizacja wysokosci
	void NormalizeGrid();


	// zwraca skrajne wartosci wysokosci Y
	float GetMaxHeight();
	float GetMinHeight();

	// generator terenu
	TerrainGenerator tGenerator;

	private:

	// bufory do rysowania terenu
	unsigned int vao, vb, ib;
	std::vector<GridVertex> Vertices;
	std::vector<unsigned int> Indices;

	//wskaznik na tablice 
	float** pGrid;


	// izicjalizacja tablicy pGrid
	void Init();
	
	// inicjalizacja buforow danych o wierzcholkach		KOLEJNOSC INICJALIZACJI: terrain.FaultFormation -> vertices -> indices -> normals
	void InitVertices(std::vector<GridVertex>& Vertices);
	void InitIndices(std::vector<unsigned int>& Indices);
	void CalcNormals(std::vector<GridVertex>& vertices, std::vector<unsigned int> indices);

	void CreateGLState();
	void PopulateBuffers();

	void NormalizeGrid(float minRange, float maxRange);

};

#endif // GRID_H