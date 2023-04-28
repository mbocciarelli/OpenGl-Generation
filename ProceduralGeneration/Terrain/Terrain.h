#pragma once
#include "HeightMap/HeightMap.h"
#include "Chunk.h"

class Terrain
{
public:
	Terrain(const Chunk chunk, NoiseSettings& settings): m_heightMap(chunk, settings)
	{
		GenerateVertices(chunk, settings);
		GenerateIndices(chunk);
	}

	void GenerateVertices(const Chunk chunk, NoiseSettings& settings)
	{
		m_vertices.clear();
		m_vertices.resize(chunk.m_width * chunk.m_height * 3);

        const float startX = chunk.m_x * chunk.m_width + chunk.m_x * -1.f;
        const float startZ = chunk.m_z * chunk.m_height + chunk.m_z * -1.f;

		for (int z = 0; z < chunk.m_height; ++z)
		{
			for (int x = 0; x < chunk.m_width; ++x)
			{
				const size_t index = (x + z * chunk.m_width) * 3;

                auto x1 = x + startX;
                auto z1 = z + startZ;

				m_vertices[index] = x1;
				m_vertices[index + 1] = m_heightMap[x + z * chunk.m_width];
				m_vertices[index + 2] = z1;
			}
		}
	}

	void GenerateIndices(const Chunk chunk)
	{
		m_indices.clear();
		m_indices.resize((chunk.m_width - 1) * (chunk.m_height - 1) * 6);
		for (int z = 0; z < chunk.m_height - 1; ++z)
		{
			for (int x = 0; x < chunk.m_width - 1; ++x)
			{
				const size_t index = (x + z * (chunk.m_width - 1)) * 6;
				m_indices[index] = x + z * chunk.m_width;
				m_indices[index + 1] = x + (z + 1) * chunk.m_width;
				m_indices[index + 2] = x + 1 + z * chunk.m_width;
				m_indices[index + 3] = x + 1 + z * chunk.m_width;
				m_indices[index + 4] = x + (z + 1) * chunk.m_width;
				m_indices[index + 5] = x + 1 + (z + 1) * chunk.m_width;
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