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
    static const particles initParticlesAB;

    static const fftw_complex densityBufferForSingleParticle[8];
    static const fftw_complex densityBufferForMultipleParticles[8];

    // for gradient tests
    static const std::vector<std::vector<double>> zeroGradient;
    static fftw_complex potential_27_data[27];
    static fftw_complex* potential_27;
    static const std::vector<std::vector<double>> gradient_27;

    // for updater tests
    static const std::vector<std::vector<double>> acceleration;
    static const std::vector<double> partilceAPos;
    static const std::vector<double> partilceASpeed;
    static const std::vector<double> partilceBPos;
    static const std::vector<double> partilceBSpeed;
};