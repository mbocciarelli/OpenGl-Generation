SET(PROCEDURAL_GENERATION_TARGET_NAME ProceduralGeneration)

add_library(${PROCEDURAL_GENERATION_TARGET_NAME} STATIC
        libs/noise/PerlinNoise.h
        PerlinNoise/PerlinGeneration.hpp
        PerlinNoise/PerlinGeneration.cpp
        )