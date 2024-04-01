#include "particles.hpp"

particles::particles(int numOfParticles, int seed)
{
    srand(seed);                               // seeding the random number generator to get the same result every time
    this->particleInfo.resize(numOfParticles); // resizing the vector to the number of particles
    for (int i = 0; i < numOfParticles; ++i)
    {
        particle particleX;
        particleX.positionSetter({rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0)}); // random position between 0 and 1
        this->particleInfo[i] = particleX;
    }
}

particles::particles(std::vector<std::vector<double>> positions)
{
    this->particleInfo.resize(positions.size()); // resizing the vector to the number of particles

    // creating a particle object for each position
    for (int i = 0; i < positions.size(); i++)
    {
        particle particleX(positions[i]);
        this->particleInfo[i] = particleX;
    }
}
