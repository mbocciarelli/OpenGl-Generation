#pragma once
#include <vector>

#include "gl/glew.h"
#include "libs/noise/PerlinNoise.h"
#include "../../libs/noise/PerlinNoise.h"
#include "../Chunk.h"

struct NoiseSettings
{
	// Define a struct to hold data for each row of the table
	struct RowData {
		float value = 0.f;
		float height = 0.f;
	};

	// Define a vector to hold all of the row data
	std::vector<RowData> splinePoints;
	float frequency = 2.f;
	int octaves = 5;
	float persistence = 0.5f;
	float minHeight = 0.f;
	float maxHeight = 200.f;
	float exponent = 1.f;
	int seed = 0;

	bool ridgeNoise = false;
	int terraceCount = 5;
};

static const NoiseSettings DesertConfig = {
        .x = 0.f,
        .y = 0.f,
        .frequency = 3.f,
        .octaves = 3,
        .persistence = 0.38f,
        .minHeight = 0.f,
        .maxHeight = 200.f,
        .exponent = 1.f,
        .seed = 0,
        .ridgeNoise = true
};

template<typename Type>
Type Map(Type val, Type in_min, Type in_max, Type out_min, Type out_max)
{
	return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

class HeightMap: public std::vector<float>
{
public:
	int mapWidth;
	int mapHeight;
	GLuint textureId = 0;

	HeightMap() = default;
	HeightMap(int mapWidth, int mapHeight, NoiseSettings& settings): mapWidth(mapWidth), mapHeight(mapHeight)
	HeightMap(const Chunk chunk, NoiseSettings& settings)
	{
		resize(chunk.m_width * chunk.m_height);
		const siv::PerlinNoise perlin(settings.seed);
		const float f = settings.frequency * 0.001f;

        const float startX = chunk.m_x * chunk.m_width + chunk.m_x * -1.f;
        const float startZ = chunk.m_z * chunk.m_height + chunk.m_z * -1.f;

		for (int z = 0; z < chunk.m_height; ++z)
		{
			for (int x = 0; x < chunk.m_width; ++x)
			{
                auto x1 = x + startX;
                auto z1 = z + startZ;

				size_t index = x + z * chunk.m_width;
				float height;
				float noise = perlin.octave2D_01((float)(x1 * f), (float)(z1 * f), settings.octaves, settings.persistence);

				if (settings.ridgeNoise) noise = Ridgenoise(noise);
				//if (settings.terraces) height = terraceNoise(noise, settings.terraceCount);
				//else height = pow(noise, settings.exponent);
				height = pow(noise, settings.exponent);

				(*this)[index] = Map(height, 0.f, 1.f, settings.minHeight, settings.maxHeight);
			}
		}
	}

	[[nodiscard]] float Ridgenoise(const float h) const
	{
		return 2 * (0.5 - abs(0.5 - h));
	}

	[[nodiscard]] float terraceNoise(const float h, const int terraceCount) const
	{
		const float terraceHeight = 1.f / terraceCount;
		return floor(h / terraceHeight) * terraceHeight;
	}

	GLuint CreateHeightMapTexture() {
		std::vector<unsigned char> textureData(mapWidth * mapHeight);

		for (int y = 0; y < mapHeight; ++y) {
			for (int x = 0; x < mapWidth; ++x) {
				const size_t index = x + y * mapWidth;
				textureData[index] = static_cast<unsigned char>((*this)[index] * 255.0f);
			}
		}

		if (textureId != 0)
		{
			glDeleteTextures(1, &textureId);
		}

		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, mapWidth, mapHeight, 0, GL_RED, GL_UNSIGNED_BYTE, textureData.data());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		return textureId;
	}
};