#pragma once

#include <memory>
#include <vector>

class VertexArray;

struct Chunk
{
    int x = 0;
    int z = 0;
    int width = 0;
    int height = 0;

    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    std::shared_ptr<VertexArray> vertexArray;
};
