//
// Created by 33644 on 27/04/2023.
//

#pragma once

#include <vector>

struct Chunk
{
    int m_x = 0;
    int m_z = 0;
    int m_width = 0;
    int m_height = 0;

    std::vector<float> m_vertices;
    std::vector<uint32_t> m_indices;
    int m_vertexArrayIndex = -1;
};