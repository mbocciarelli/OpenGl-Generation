#pragma once
#include <vector>
#include "gl/glew.h"
#include "../../libs/noise/PerlinNoise.h"
#include "../../PerlinNoise/PerlinGeneration.h"

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
	HeightMap(const int width, const int height, const int x, const int z, NoiseSettings& continentalnessSettings, NoiseSettings& erosionSettings, bool blend): mapWidth(width), mapHeight(height)
	{
		resize(width * height);
		const siv::PerlinNoise continentalnessPerlin(continentalnessSettings.seed);
		const siv::PerlinNoise erosionPerlin(erosionSettings.seed);

        const float startX = x * width + x * -1.f;
        const float startZ = z * height + z * -1.f;

		for (int z = 0; z < height; ++z)
		{
			for (int x = 0; x < width; ++x)
			{
                auto x1 = x + startX;
                auto z1 = z + startZ;

				size_t index = x + z * width;

				const float f = continentalnessSettings.frequency * 0.001f;
				float continentalnessNoise = continentalnessSettings.GetNoiseValue(continentalnessPerlin, x1 * f, z1 * f);

				if (continentalnessSettings.splinePoints.size() > 1)
				{
					auto previousSpline = continentalnessSettings.splinePoints[0];
					auto currentSpline = continentalnessSettings.splinePoints[0];
					for (int i = 0; i < continentalnessSettings.splinePoints.size(); ++i)
					{
						previousSpline = currentSpline;
						currentSpline = continentalnessSettings.splinePoints[i];
						if (continentalnessNoise < continentalnessSettings.splinePoints[i].value)
						{
							break;
						}
					}
					continentalnessNoise = Map(continentalnessNoise, previousSpline.value, currentSpline.value, previousSpline.height, currentSpline.height);
				}

				const float g = erosionSettings.frequency * 0.001f;
				float erosionNoise = erosionSettings.GetNoiseValue(erosionPerlin, x1 * g, z1 * g);

				if (erosionSettings.splinePoints.size() > 1)
				{
					auto previousSpline = erosionSettings.splinePoints[0];
					auto currentSpline = erosionSettings.splinePoints[0];
					for (int i = 0; i < erosionSettings.splinePoints.size(); ++i)
					{
						previousSpline = currentSpline;
						currentSpline = erosionSettings.splinePoints[i];
						if (erosionNoise < erosionSettings.splinePoints[i].value)
						{
							break;
						}
					}
					erosionNoise = Map(erosionNoise, previousSpline.value, currentSpline.value, previousSpline.height, currentSpline.height);
				}


				if (continentalnessSettings.ridgeNoise) {
					continentalnessNoise = Ridgenoise(continentalnessNoise);
					erosionNoise = Ridgenoise(erosionNoise);
				}

				if (continentalnessSettings.terraces) {
					continentalnessNoise = terraceNoise(continentalnessNoise, continentalnessSettings.terraceCount);
					erosionNoise = terraceNoise(erosionNoise, erosionSettings.terraceCount);
				}

				float height;

				if (blend) {
					height = BlendWithSubstractiveErosionNoise(continentalnessNoise, erosionNoise, erosionSettings.factor);
				} else
				{
					height = continentalnessNoise;
				}

				(*this)[index] = height;
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mapWidth, mapHeight, 0, GL_RED, GL_UNSIGNED_BYTE, textureData.data());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);

		return textureId;
	}


	float BlendWithSubstractiveErosionNoise(float v1, float erosionNoise, float erosionFactor) {
		const float erosionValue = erosionNoise * erosionFactor;
		auto v =  std::max(0.f, std::min(v1 - erosionValue, v1));
		return v;
	}

};