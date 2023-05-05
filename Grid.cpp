#include "Grid.h"
#include "Shader.h"

// utility dla struct vertex - inicjalizacja wierzcho�ka
void GridVertex::InitVertex(int x, int y, int z, int Width, int Depth, float worldScale)
{
	Pos = glm::vec3(x * worldScale, y, z * worldScale);
	Tex = glm::vec2((float)x / Width, (float)z / Depth);
	Tex *= worldScale;
}



Grid::Grid(int width, int depth, float worldscale,float heightScaleFactor, float minH, float maxH, int iter, float filter)
{
	WorldScale = worldscale;
	Width = width;
	Depth = depth;
	MinHeight = minH;
	MaxHeight = maxH;
	HeightScaleFactor = heightScaleFactor;

	Init();

	//fault formation
    tGenerator.CreateFaultFormation(*this, iter, filter);
	
	CreateGLState();
}

Grid::~Grid()
{
	for (int i = 0; i < Width; i++)
	{
		delete pGrid[i];
	}
	delete pGrid;
}

float Grid::GetGridPoint(int x, int z)
{
	return pGrid[x][z];
}

void Grid::SetGridPoint(int x, int z, float newVal)
{
	pGrid[x][z] = newVal;
}

void Grid::Init()
{
	pGrid = new float*[Width];

	for (int i = 0; i < Width; i++)
	{
		pGrid[i] = new float[Depth];
	}

	// zainicjuj poczatkowe wartosci

	for (int w = 0; w < Width; w++)
	{
		for (int d = 0; d < Depth; d++)
		{
			pGrid[w][d] = 0.0f;
		}
	}
}

void Grid::CreateGLState()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glGenBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);

	PopulateBuffers();

	//POSITION
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GridVertex), (void*)(0 * sizeof(float)));
	glEnableVertexAttribArray(0);

	//TEX
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GridVertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//NORMAL
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GridVertex), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Grid::PopulateBuffers()
{
	Vertices.resize(Width * Depth);
	InitVertices(Vertices);


	int NumQuads = (Width - 1) * (Depth - 1);
	Indices.resize(NumQuads * 6);
	InitIndices(Indices);


	CalcNormals(Vertices, Indices);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0]) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

}

void Grid::NormalizeGrid(float minRange, float maxRange)
{
	//find min and max
	float min = 9999999999.0, max = -9999999999.0f;

	float height;

	for (int z = 0; z < Depth; z++)
	{
		for (int x = 0; x < Width; x++)
		{

			height = GetGridPoint(x, z);

			if (height > max)
			{
				max = height;
			}
			if (height < min)
			{
				min = height;
			}


		}
	}

	assert(max < min);

	float minMaxDelta = max - min;
	float minMaxRange = maxRange - minRange;

	float newVal;
	for (int z = 0; z < Depth; z++)
	{
		for (int x = 0; x < Width; x++)
		{
			newVal = ((GetGridPoint(x,z) - min) / minMaxDelta) * minMaxRange + minRange;
			SetGridPoint(x, z, newVal);
		}
	}


}

void Grid::NormalizeGrid(){
	
	NormalizeGrid(MinHeight* HeightScaleFactor, MaxHeight* HeightScaleFactor);
}

void Grid::InitVertices(std::vector<GridVertex>& Vertices)
{
	int Index = 0;
	for (int z = 0; z < Depth; z++)
	{
		for (int x = 0; x < Width; x++)
		{
			assert(Index < Vertices.size());

			float y = GetGridPoint(x,z);
			Vertices[Index].InitVertex(x, y, z, Width, Depth, WorldScale);
			Index++;
		}
	}

}

void Grid::InitIndices(std::vector<unsigned int>& Indices)
{
	int Index = 0;
	for (int z = 0; z < Depth - 1; z++)
	{
		for (int x = 0; x < Width - 1; x++)
		{
			unsigned int IndexBottomLeft = z * Width + x;
			unsigned int IndexTopLeft = (z + 1) * Width + x;
			unsigned int IndexTopRight = (z + 1) * Width + x + 1;
			unsigned int IndexBottomRight = z * Width + x + 1;

			// Add top left triangle
			assert(Index < Indices.size());
			Indices[Index++] = IndexBottomLeft;
			assert(Index < Indices.size());
			Indices[Index++] = IndexTopLeft;
			assert(Index < Indices.size());
			Indices[Index++] = IndexTopRight;

			// Add bottom right triangle
			assert(Index < Indices.size());
			Indices[Index++] = IndexBottomLeft;
			assert(Index < Indices.size());
			Indices[Index++] = IndexTopRight;
			assert(Index < Indices.size());
			Indices[Index++] = IndexBottomRight;

		}
	}
}

void Grid::CalcNormals(std::vector<GridVertex>& vertices, std::vector<unsigned int> indices)
{

	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		unsigned int index0 = indices[i];
		unsigned int index1 = indices[i + 1];
		unsigned int index2 = indices[i + 2];

		glm::vec3 v1 = vertices[index1].Pos - vertices[index0].Pos;
		glm::vec3 v2 = vertices[index2].Pos - vertices[index0].Pos;




		glm::vec3 normal = glm::vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
		float l = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		normal.x = normal.x / l;
		normal.y = normal.y / l;
		normal.z = normal.z / l;

		vertices[index0].Normal += normal;
		vertices[index1].Normal += normal;
		vertices[index2].Normal += normal;

	}

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		float l = sqrt(vertices[i].Normal.x * vertices[i].Normal.x + vertices[i].Normal.y * vertices[i].Normal.y + vertices[i].Normal.z * vertices[i].Normal.z);
		vertices[i].Normal.x = vertices[i].Normal.x / l;
		vertices[i].Normal.y = vertices[i].Normal.y / l;
		vertices[i].Normal.z = vertices[i].Normal.z / l;

	}
}

void Grid::Generate2RandGridPoints(GridPoint& p1, GridPoint& p2)
{

	p1.x = rand() % Width;
	p2.z = rand() % Depth;

	do {

		p2.x = rand() % Width;
		p2.z = rand() % Depth;

	} while (p1.IsEqual(p2));
}

void Grid::Draw(Shader& shader)
{
	shader.use();

	//height level textures data to shader
	shader.setFloat("lv0", tGenerator.GetTextureLevel(0));
	shader.setFloat("lv1", tGenerator.GetTextureLevel(1));
	shader.setFloat("lv2", tGenerator.GetTextureLevel(2));
	shader.setFloat("lv3", tGenerator.GetTextureLevel(3));


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tGenerator.terrainTextureLv0);
	shader.setInt("terrainTextureLv0", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tGenerator.terrainTextureLv1);
	shader.setInt("terrainTextureLv1", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tGenerator.terrainTextureLv2);
	shader.setInt("terrainTextureLv2", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tGenerator.terrainTextureLv3);
	shader.setInt("terrainTextureLv3", 3);


	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

float Grid::GetMinHeight()
{

	float min = 99999999999.0f;
	for (int x = 0; x < Width; x++)
	{
		for (int z = 0; z < Depth; z++)
		{
			if (GetGridPoint(x, z) < min)
				min = GetGridPoint(x, z);
		}
	}
	return min;
}

float Grid::GetMaxHeight()
{

	float max = -99999999999.0f;
	for (int x = 0; x < Width; x++)
	{
		for (int z = 0; z < Depth; z++)
		{
			if (GetGridPoint(x, z) > max)
				max = GetGridPoint(x, z);
		}
	}
	return max;
}

GridVertex Grid::GetGridVertex(int index)
{
	assert(index <= Vertices.size());
	return Vertices[index];
}