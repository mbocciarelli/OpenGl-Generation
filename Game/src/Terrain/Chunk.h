#pragma once

#include <memory>
#include <vector>

#include "HeightMap/HeightMap.h"


class VertexArray;
struct NoiseSettings;


class Chunk
{
public:
    Chunk(int x, int z, int width, int height, int lod, NoiseSettings& continentalnessSettings, NoiseSettings& erosionSettings, bool blend): x(x), z(z), width(width), height(height), lod(lod), m_heightMap(width, height, x, z, lod, continentalnessSettings, erosionSettings, blend)
    {
		GenerateVertices();
		GenerateIndices();
    }

	HeightMap& GetHeightMap()
	{
		return m_heightMap;
	}

	void SetHeightMap(const HeightMap& heightMap)
	{
		m_heightMap = heightMap;
	}

	void SetVertexArray(const std::shared_ptr<VertexArray>& vertexArray)
    {
	    m_vertexArray = vertexArray;
    }

	std::vector<float>& GetVertices()
	{
		return m_vertices;
	}

	std::vector<uint32_t>& GetIndices()
	{
		return m_indices;
	}

	const std::shared_ptr<VertexArray>& GetVertexArray()
	{
		return m_vertexArray;
	}

private:

	void GenerateVertices()
	{
		m_vertices.clear();
		m_vertices.resize(width * lod * height * lod * 3);

		const float startX = x * width + x * -1.f;
		const float startZ = z * height + z * -1.f;

		for (int z = 0; z < height * lod; ++z)
		{
			for (int x = 0; x < width * lod; ++x)
			{
				const size_t index = (x + z * width * lod) * 3;

				auto x1 = x / (float)lod + startX;
				auto z1 = z / (float)lod + startZ;

				m_vertices[index] = x1;
				m_vertices[index + 1] = m_heightMap[x + z * width * lod];
				m_vertices[index + 2] = z1;
			}
		}
	}

	void GenerateIndices()
	{
		m_indices.clear();
		m_indices.resize((width * lod - 1 * lod) * (height * lod - 1 * lod) * 6);
		for (int z = 0; z < height * lod - 1 * lod; ++z)
		{
			for (int x = 0; x < width * lod - 1 * lod; ++x)
			{
				const size_t index = (x + z * (width * lod - 1 * lod)) * 6;
				m_indices[index] = x + z * width * lod;
				m_indices[index + 1] = x + (z + 1) * width * lod;
				m_indices[index + 2] = x + 1 + z * width * lod;
				m_indices[index + 3] = x + 1 + z * width * lod;
				m_indices[index + 4] = x + (z + 1) * width * lod;
				m_indices[index + 5] = x + 1 + (z + 1) * width * lod;
			}
		}
	}

public:
    int x = 0;
    int z = 0;
    int width = 0;
    int height = 0;
	int lod = 1;

private:
    std::vector<float> m_vertices;
    std::vector<uint32_t> m_indices;

	HeightMap m_heightMap;
    std::shared_ptr<VertexArray> m_vertexArray;


};
