#pragma once
#include <vector>
#include "libs/noise/PerlinNoise.h"

struct NoiseSettings
{
	float x = 0.f;
	float y = 0.f;
	float frequency = 2.f;
	int octaves = 5;
	float persistence = 0.5f;
	float minHeight = 0.f;
	float maxHeight = 200.f;
	float exponent = 1.f;
	int seed = 0;

	bool ridgeNoise = false;
	bool terraces = false;
	int terraceCount = 5;
};

template<typename Type>
Type Map(Type val, Type in_min, Type in_max, Type out_min, Type out_max)
{
	return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

class HeightMap: public std::vector<float>
{
public:
	HeightMap(int mapWidth, int mapHeight, NoiseSettings& settings)
	{
		resize(mapWidth * mapHeight);
		const siv::PerlinNoise perlin(settings.seed);
		const float f = settings.frequency * 0.001f;
		for (int z = 0; z < mapHeight; ++z)
		{
			for (int x = 0; x < mapWidth; ++x)
			{
				size_t index = x + z * mapWidth;
				float height;
				float noise = perlin.octave2D_01((float)(x * f), (float)(z * f), settings.octaves, settings.persistence);

				if (settings.ridgeNoise) noise = Ridgenoise(noise);
				if (settings.terraces) height = terraceNoise(height, settings.terraceCount);
				else height = pow(noise, settings.exponent);

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
};