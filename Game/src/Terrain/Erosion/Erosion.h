#pragma once
#include <random>
#include <vector>

struct HeightAndGradient
{
	float height;
	float gradientX;
	float gradientY;
};

struct ErosionSettings
{
	bool enable = false;
	int seed = 1;
	int erosionRadius = 3;
	float inertia = 0.05f;
	float sedimentCapacityFactor = 4.0f;
	float minSedimentCapacity = 0.01f;
	float erodeSpeed = 0.3f;
	float depositSpeed = 0.3f;
	float evaporateSpeed = 0.01f;
	float gravity = 4.0f;
	int maxDropletLifetime = 30;
	float initialWaterVolume = 1.0f;
	float initialSpeed = 2.0f;
};

class Erosion
{
public:
	Erosion(ErosionSettings& settings);

	void Erode(std::vector<float>& map, int mapSize, int numIterations);

private:
	void Initialize(int mapSize);

	HeightAndGradient CalculateHeightAndGradient(const std::vector<float>& nodes, int mapSize, float posX, float posY);

	void InitializeBrushIndices(int mapSize, int radius);

private:
	bool m_enable;
	int m_seed;
	int m_erosionRadius;
	float m_inertia;
	float m_sedimentCapacityFactor;
	float m_minSedimentCapacity;
	float m_erodeSpeed;
	float m_depositSpeed;
	float m_evaporateSpeed;
	float m_gravity;
	int m_maxDropletLifetime;

	float m_initialWaterVolume;
	float m_initialSpeed;

	std::vector<std::vector<int>> m_erosionBrushIndices;
	std::vector<std::vector<float>> m_erosionBrushWeights;

	std::mt19937 prng;
};

