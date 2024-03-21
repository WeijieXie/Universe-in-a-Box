#pragma once

#include "particle.hpp"

#include <cstdlib>

class particles
{
public:
    std::vector<particle> particleInfo;

    particles(int numOfParticles, int seed);
    particles(std::vector<std::vector<double>> positions);
};