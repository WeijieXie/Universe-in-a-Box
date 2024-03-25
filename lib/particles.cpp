#include "particles.hpp"

particles::particles(int numOfParticles, int seed)
{
    srand(seed);
    // this->particleInfo.clear();
    for (int i = 0; i < numOfParticles; ++i)
    {
        particle particleX;
        particleX.positionSetter({rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0)});
        this->particleInfo.push_back(particleX);
    }
}

particles::particles(std::vector<std::vector<double>> positions)
{
    // this->particleInfo.clear();
    for (auto iter = positions.begin(); iter < positions.end(); iter++)
    {
        particle particleX(*iter);
        this->particleInfo.push_back(particleX);
    }
}
