#pragma once

#include "particles.hpp"

#include <fftw3.h>
#include <cstring>

class Simulation
{
public:
    double timeMax;
    double timeStep;
    particles particlesSimu;
    double width;
    int numOfCellsPerDim;
    double expanFac;

    int numOfCells;
    double volOfBox;
    double volOfCell;
    double relCellWidth; // which is 1.0/numOfCellsPerDim
    double densityContributionPerParticle;

    fftw_complex *densityBuffer = nullptr;
    fftw_complex *potentialBuffer = nullptr;
    fftw_complex *frequencyBuffer = nullptr;

    fftw_plan forward_plan;
    fftw_plan inverse_plan;

    Simulation(double timeMax, double timeStep, particles initParticles, double width, int numOfCellsPerDim, double expanFac);
    ~Simulation();

    void densityCalculator();

    /**
     * @brief Run a particle mesh simulation from t=0 to t_max
     *
     */
    void run();
};