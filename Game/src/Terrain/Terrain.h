#pragma once
#include "HeightMap/HeightMap.h"

class Terrain
{
public:
	Terrain(const int width, const int height, NoiseSettings& settings): m_heightMap(width, height, settings)
	{
		GenerateVertices(width, height, settings);
		GenerateIndices(width, height);
	}

	void GenerateVertices(const int width, const int height, NoiseSettings& settings)
	{
		m_vertices.clear();
		m_vertices.resize(width * height * 3);
		for (int z = 0; z < height; ++z)
		{
			for (int x = 0; x < width; ++x)
			{
				const size_t index = (x + z * width) * 3;
				m_vertices[index] = x;
				m_vertices[index + 1] = m_heightMap[x + z * width];
				m_vertices[index + 2] = z;
			}
		}
	}

	void GenerateIndices(const int width, const int height)
	{
		m_indices.clear();
		m_indices.resize((width - 1) * (height - 1) * 6);
		for (int z = 0; z < height - 1; ++z)
		{
			for (int x = 0; x < width - 1; ++x)
			{
				const size_t index = (x + z * (width - 1)) * 6;
				m_indices[index] = x + z * width;
				m_indices[index + 1] = x + (z + 1) * width;
				m_indices[index + 2] = x + 1 + z * width;
				m_indices[index + 3] = x + 1 + z * width;
				m_indices[index + 4] = x + (z + 1) * width;
				m_indices[index + 5] = x + 1 + (z + 1) * width;
			}
		}
	}


	HeightMap& GetHeightMap()
	{
		return m_heightMap;
	}

	void SetHeightMap(const HeightMap& heightMap)
	{
		m_heightMap = heightMap;
	}

	std::vector<float>& GetVertices()
	{
		return m_vertices;
	}

	std::vector<uint32_t>& GetIndices()
	{
		return m_indices;
	}

private:
	HeightMap m_heightMap;
	std::vector<float> m_vertices;
	std::vector<uint32_t> m_indices;

};