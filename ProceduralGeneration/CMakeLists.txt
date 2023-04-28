SET(PROCEDURAL_GENERATION_TARGET_NAME ProceduralGeneration)

add_library(${PROCEDURAL_GENERATION_TARGET_NAME} STATIC
        libs/noise/PerlinNoise.h
        PerlinNoise/PerlinGeneration.hpp
        PerlinNoise/PerlinGeneration.cpp
        Terrain/Terrain.h
        Terrain/HeightMap/HeightMap.h
        Terrain/Erosion/Erosion.h
        Terrain/Erosion/Erosion.cpp
        Terrain/Chunk.cpp Terrain/Chunk.h)