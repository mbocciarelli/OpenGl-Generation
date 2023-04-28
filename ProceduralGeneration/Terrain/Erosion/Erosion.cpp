#include "Erosion.h"

// NIKE LA REFACTO
Erosion::Erosion(ErosionSettings& settings): m_enable(settings.enable), m_seed(settings.seed), m_erosionRadius(settings.erosionRadius), m_initialSpeed(settings.initialSpeed), m_initialWaterVolume(settings.initialWaterVolume), m_inertia(settings.inertia), m_sedimentCapacityFactor(settings.sedimentCapacityFactor), m_minSedimentCapacity(settings.minSedimentCapacity), m_depositSpeed(settings.depositSpeed), m_erodeSpeed(settings.erodeSpeed), m_evaporateSpeed(settings.evaporateSpeed), m_gravity(settings.gravity), m_maxDropletLifetime(settings.maxDropletLifetime)
{}


void Erosion::Initialize(int mapSize) {
    prng = std::mt19937(m_seed);
    InitializeBrushIndices(mapSize, m_erosionRadius);
}


void Erosion::Erode(std::vector<float>& map, int mapSize, int numIterations) {
    if (!m_enable) return;
    Initialize(mapSize);
    std::uniform_int_distribution<int> dist(0, mapSize - 2);

    for (int iteration = 0; iteration < numIterations; iteration++) {
        float posX = dist(prng);
        float posY = dist(prng);
        float dirX = 0;
        float dirY = 0;
        float speed = m_initialSpeed;
        float water = m_initialWaterVolume;
        float sediment = 0;

        for (int lifetime = 0; lifetime < m_maxDropletLifetime; lifetime++) {
            int nodeX = static_cast<int>(posX);
            int nodeY = static_cast<int>(posY);
            int dropletIndex = nodeY * mapSize + nodeX;
            float cellOffsetX = posX - nodeX;
            float cellOffsetY = posY - nodeY;

            HeightAndGradient heightAndGradient = CalculateHeightAndGradient(map, mapSize, posX, posY);

            dirX = (dirX * m_inertia - heightAndGradient.gradientX * (1 - m_inertia));
            dirY = (dirY * m_inertia - heightAndGradient.gradientY * (1 - m_inertia));
            float len = std::sqrt(dirX * dirX + dirY * dirY);
            if (len != 0) {
                dirX /= len;
                dirY /= len;
            }
            posX += dirX;
            posY += dirY;

            if ((dirX == 0 && dirY == 0) || posX < 0 || posX >= mapSize - 2 || posY < 0 || posY >= mapSize - 2) {
                break;
            }

            float newHeight = CalculateHeightAndGradient(map, mapSize, posX, posY).height;
            float deltaHeight = newHeight - heightAndGradient.height;

            speed = std::isnan(speed) ? 0.0f : speed;
            float sedimentCapacity = std::max(-deltaHeight * speed * water * m_sedimentCapacityFactor, m_minSedimentCapacity);

            if (sediment > sedimentCapacity || deltaHeight > 0) {
                float amountToDeposit = (deltaHeight > 0) ? std::min(deltaHeight, sediment) : (sediment - sedimentCapacity) * m_depositSpeed;
                sediment -= amountToDeposit;
                map[dropletIndex] += amountToDeposit * (1 - cellOffsetX) * (1 - cellOffsetY);
                map[dropletIndex + 1] += amountToDeposit * cellOffsetX * (1 - cellOffsetY);
                map[dropletIndex + mapSize] += amountToDeposit * (1 - cellOffsetX) * cellOffsetY;
                map[dropletIndex + mapSize + 1] += amountToDeposit * cellOffsetX * cellOffsetY;

            }
            else {
                float amountToErode = std::min((sedimentCapacity - sediment) * m_erodeSpeed, -deltaHeight);

                for (int brushPointIndex = 0; brushPointIndex < m_erosionBrushIndices[dropletIndex].size(); brushPointIndex++) {
                    int nodeIndex = m_erosionBrushIndices[dropletIndex][brushPointIndex];
                    float weighedErodeAmount = amountToErode * m_erosionBrushWeights[dropletIndex][brushPointIndex];
                    float deltaSediment = (map[nodeIndex] < weighedErodeAmount) ? map[nodeIndex] : weighedErodeAmount;
                    map[nodeIndex] -= deltaSediment;
                    sediment += deltaSediment;
                }
            }

            speed = std::sqrt(speed * speed + deltaHeight * m_gravity);
        	water *= (1 - m_evaporateSpeed);
        }
    }
}

HeightAndGradient Erosion::CalculateHeightAndGradient(const std::vector<float>& nodes, int mapSize, float posX, float posY) {
    int coordX = static_cast<int>(posX);
    int coordY = static_cast<int>(posY);
    float x = posX - coordX;
    float y = posY - coordY;

    int nodeIndexNW = coordY * mapSize + coordX;
    float heightNW = nodes[nodeIndexNW];
    float heightNE = nodes[nodeIndexNW + 1];
    float heightSW = nodes[nodeIndexNW + mapSize];
    float heightSE = nodes[nodeIndexNW + mapSize + 1];

    float gradientX = (heightNE - heightNW) * (1 - y) + (heightSE - heightSW) * y;
    float gradientY = (heightSW - heightNW) * (1 - x) + (heightSE - heightNE) * x;

    float height = heightNW * (1 - x) * (1 - y) + heightNE * x * (1 - y) + heightSW * (1 - x) * y + heightSE * x * y;

    return { height, gradientX, gradientY };
}

void Erosion::InitializeBrushIndices(int mapSize, int radius) {
    m_erosionBrushIndices.resize(mapSize * mapSize);
    m_erosionBrushWeights.resize(mapSize * mapSize);
    std::vector<int> xOffsets(radius * radius * 4);
    std::vector<int> yOffsets(radius * radius * 4);
    std::vector<float> weights(radius * radius * 4);
    float weightSum = 0.f;
    int addIndex = 0;

    for (int i = 0; i < m_erosionBrushIndices.size(); i++) {
        int centreX = i % mapSize;
        int centreY = i / mapSize;

        if (centreY <= radius || centreY >= mapSize - radius || centreX <= radius + 1 || centreX >= mapSize - radius) {
            weightSum = 0.f;
            addIndex = 0;
            for (int y = -radius; y <= radius; y++) {
                for (int x = -radius; x <= radius; x++) {
                    float sqrDst = x * x + y * y;
                    if (sqrDst < radius * radius) {
                        int coordX = centreX + x;
                        int coordY = centreY + y;
                        if (coordX >= 0 && coordX < mapSize && coordY >= 0 && coordY < mapSize) {
                            float weight = 1.f - std::sqrt(sqrDst) / radius;
                            weightSum += weight;
                            weights[addIndex] = weight;
                            xOffsets[addIndex] = x;
                            yOffsets[addIndex] = y;
                            addIndex++;
                        }
                    }
                }
            }
        }

        int numEntries = addIndex;
        m_erosionBrushIndices[i].resize(numEntries);
        m_erosionBrushWeights[i].resize(numEntries);

        for (int j = 0; j < numEntries; j++) {
            m_erosionBrushIndices[i][j] = (yOffsets[j] + centreY) * mapSize + xOffsets[j] + centreX;
            m_erosionBrushWeights[i][j] = weights[j] / weightSum;
        }
    }
}