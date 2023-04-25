//
// Created by Admin on 24/04/2023.
//

#pragma once

#include "../libs/noise/PerlinNoise.h"

struct NoiseSettings
{
    float frequency = 2.f;
    int octaves = 5;
    float persistence = 0.5f;

    float exponent = 3.f;
    int seed = 5203;

    float minHeight = 0.f;
    float maxHeight = 200.f;

    [[nodiscard]] float GetNoiseValue(const siv::PerlinNoise& perlin, float x, float z, const float f) const
    {
        return perlin.octave2D_01(x * f, z * f, octaves, persistence);
    }

    template<typename Type>
    Type RangeMax(Type val, Type in_min, Type in_max, Type out_min, Type out_max) const
    {
        return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
};


class PerlinGeneration {

public:
    static void GenerateHeightMap(std::vector<float>& vertices, std::vector<uint32_t>& indices, int size, const NoiseSettings& settings)
    {
        const siv::PerlinNoise perlin(settings.seed);
        const float f = (settings.frequency / (float)size);
        for (int z = 0; z < size; ++z)
        {
            for (int x = 0; x < size; ++x)
            {
                size_t index = (x + z * size) * 3;
                float height = settings.GetNoiseValue(perlin, x, z, f);
                height = pow(height, 3.f);
                vertices[index] = x;
                vertices[index + 1] = settings.RangeMax(height, 0.f, 1.f, settings.minHeight, settings.maxHeight);
                vertices[index + 2] = z;
            }
        }
        int indexIndice = 0;
        for (int z = 0; z < size - 1; ++z)
        {
            for (int x = 0; x < size - 1; ++x)
            {
                int topLeft = (x + z * size);
                int topRight = topLeft + 1;
                int bottomLeft = topLeft + size;
                int bottomRight = bottomLeft + 1;

                indices[indexIndice++] = topLeft;
                indices[indexIndice++] = bottomLeft;
                indices[indexIndice++] = topRight;
                indices[indexIndice++] = topRight;
                indices[indexIndice++] = bottomLeft;
                indices[indexIndice++] = bottomRight;
            }
        }
    }

};
