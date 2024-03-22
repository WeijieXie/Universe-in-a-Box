#pragma once

#include "Simulation.hpp"

class test_data
{
public:
    static const double timeMax;
    static const double timeStep;
    static const double mass;
    static const double width;
    static const double expanFac;

    static const int numOfCellsPerDim;
    static const double densityContributionPerParticle;

    static const particles initParticlesEmpty;
    static const particles initParticlesRandom;
    static const particles initParticlesOneInMid;
    static const particles initParticlesMultiple;

    static const fftw_complex densityBufferForSingleParticle[8];
    static const fftw_complex densityBufferForMultipleParticles[8];
};