#pragma once

#include "particle.hpp"

#include <cstdlib>

class particles
{
public:
    std::vector<particle> particleInfo = {};

    particles() = default;
    particles(int numOfParticles, int seed);               // constructor for random positions
    particles(std::vector<std::vector<double>> positions); // constructor for given positions
};