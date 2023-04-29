#pragma once

#include "../libs/noise/PerlinNoise.h"

struct NoiseSettings
{
    // Define a struct to hold data for each row of the table
    struct RowData {
        float value = 0.f;
        float height = 0.f;
    };

    // Define a vector to hold all of the row data
    std::vector<RowData> splinePoints;

    float frequency = 4.5f;
    int octaves = 5;
    float persistence = 0.5f;

    float exponent = 1.f;
    int seed = 0;

    float minHeight = 0.f;
    float maxHeight = 200.f;

    bool ridgeNoise = false;
    bool terraces = false;
    int terraceCount = 5;

    [[nodiscard]] float GetNoiseValue(const siv::PerlinNoise& perlin, float x, float z) const
    {
        return perlin.octave2D(x, z, octaves, persistence);
    }

    template<typename Type>
    Type RangeMax(Type val) const
    {
        return (val - -1.f) * (maxHeight - minHeight) / (1.f - -1.f) + minHeight;
    }
};
//
//
//class PerlinGeneration {
//
//public:
//    static void GenerateHeightMap(std::vector<float>& vertices, std::vector<uint32_t>& indices, int size, const NoiseSettings& settings)
//    {
//        const siv::PerlinNoise perlin(settings.seed);
//        const float f = settings.frequency * 0.001f;
//        for (int z = 0; z < size; ++z)
//        {
//            for (int x = 0; x < size; ++x)
//            {
//                size_t index = (x + z * size) * 3;
//                float height = settings.GetNoiseValue(perlin, x * f, z * f);
//                height = pow(height, 3.f);
//                vertices[index] = x;
//                vertices[index + 1] = settings.RangeMax(height);
//                vertices[index + 2] = z;
//            }
//        }
//        int indexIndice = 0;
//        for (int z = 0; z < size - 1; ++z)
//        {
//            for (int x = 0; x < size - 1; ++x)
//            {
//                int topLeft = (x + z * size);
//                int topRight = topLeft + 1;
//                int bottomLeft = topLeft + size;
//                int bottomRight = bottomLeft + 1;
//
//                indices[indexIndice++] = topLeft;
//                indices[indexIndice++] = bottomLeft;
//                indices[indexIndice++] = topRight;
//                indices[indexIndice++] = topRight;
//                indices[indexIndice++] = bottomLeft;
//                indices[indexIndice++] = bottomRight;
//            }
//        }
//    }
//
//};
