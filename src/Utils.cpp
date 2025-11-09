#include "Utils.h"
#include <random>

// Restituisce un generatore condiviso
inline std::mt19937 &GetRNG()
{
    static std::mt19937 gen(std::random_device{}()); // inizializzato solo una volta
    return gen;
}
float sampleGaussian(float mean, float std)
{
    std::normal_distribution<float> dist(mean = mean, std = std);
    return dist(GetRNG());
}