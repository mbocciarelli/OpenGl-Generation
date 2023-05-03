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
    float factor = 0.5f;

    int seed = 0;

    bool ridgeNoise = false;
    bool terraces = false;
    int terraceCount = 5;

    [[nodiscard]] float GetNoiseValue(const siv::PerlinNoise& perlin, float x, float z) const
    {
        return perlin.octave2D(x, z, octaves, persistence);
    }
};


const NoiseSettings continentalnessNoiseSettings{
    .splinePoints = {
        {-1.f, 10.f},
        { -0.5f, 20.f },
        {-0.4, 40.f },
        { -0.2f, 44.f },
        {-0.15f, 60.f},
        {-0.1f, 60.f},
        {0.f, 70.f},
        {0.2f, 80.f},
        {1.f, 10.f}
        },
    .frequency = 0.001f,
    .octaves = 5,
    .persistence = 0.5f,
    .factor = 1.f,
    .seed = 0,
    .ridgeNoise = false,
    .terraces = false,
    .terraceCount = 1
};

const NoiseSettings erosionNoiseSettings{
    .splinePoints = {
        {-1.f, 50.f},
        { -0.4f, 50.f },
        {-0.5, 40.f },
        { -0.1f, 10.f },
        {0.3f, 10.f},
        {0.5f, 10.f},
        {0.55f, 20.f},
        {0.6f, 30.f},
        {0.61f, 50.f},
        {1.f, 10.f},
        },
    .frequency = 0.01f,
    .octaves = 2,
    .persistence = 0.5f,
    .factor = 0.5f,
    .seed = 0,
    .ridgeNoise = false,
    .terraces = false,
    .terraceCount = 1
};