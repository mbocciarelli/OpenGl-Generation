#pragma once
#include <vector>

#include "gl/glew.h"
#include "../../libs/noise/PerlinNoise.h"
#include "../Chunk.h"

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
	HeightMap(const Chunk chunk, NoiseSettings& settings): mapWidth(chunk.width), mapHeight(chunk.height)
	{
		resize(chunk.width * chunk.height);
		const siv::PerlinNoise perlin(settings.seed);
		const float f = settings.frequency * 0.001f;

        const float startX = chunk.x * chunk.width + chunk.x * -1.f;
        const float startZ = chunk.z * chunk.height + chunk.z * -1.f;

		for (int z = 0; z < chunk.height; ++z)
		{
			for (int x = 0; x < chunk.width; ++x)
			{
                auto x1 = x + startX;
                auto z1 = z + startZ;

				size_t index = x + z * chunk.width;
				float height;
				float noise = settings.GetNoiseValue(perlin, x1 * f, z1 * f);

				if (settings.ridgeNoise) noise = Ridgenoise(noise);
				if (settings.terraces) height = terraceNoise(noise, settings.terraceCount);
				else height = pow(noise, settings.exponent);
				height = pow(noise, settings.exponent);

				(*this)[index] = settings.RangeMax(height);
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