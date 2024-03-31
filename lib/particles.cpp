#include "particles.hpp"

particles::particles(int numOfParticles, int seed)
{
    srand(seed);
    this->particleInfo.resize(numOfParticles);
    for (int i = 0; i < numOfParticles; ++i)
    {
        particle particleX;
        particleX.positionSetter({rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0)});
        this->particleInfo[i] = particleX;
    }
}

particles::particles(std::vector<std::vector<double>> positions)
{
    this->particleInfo.resize(positions.size());
    for (int i = 0; i < positions.size(); i++)
    {
        particle particleX(positions[i]);
        this->particleInfo[i] = particleX;
    }
}
