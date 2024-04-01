#pragma once

#include "particles.hpp"
#include "Utils.hpp"

#include <fftw3.h>
#include <cstring>
#include <optional>
#include <iostream>

#include <filesystem>
#include <sstream>

#include <omp.h>
#include <chrono>
#include <thread>

class Simulation
{
public:
    double timeMax;
    double timeStep;
    double width;
    int numOfCellsPerDim;
    double expanFac;

    int numOfCells;
    int numOfParticles;
    double volOfBox;
    double volOfCell;
    double relCellWidth; // which is 1.0/numOfCellsPerDim
    double cellWidth;
    double densityContributionPerParticle; // the contribution of a single particle to the density of a cell
    double wSquare;                        // the square of the width
    std::vector<double> kSquare;           // the square of the wave vector
    const double PI = 3.141592653589793238463;
    double fNorm;

    particles particlesSimu;

    fftw_complex *densityBuffer = nullptr;
    fftw_complex *potentialBuffer = nullptr;
    fftw_complex *frequencyBuffer = nullptr;
    std::vector<std::vector<double>> acceleration;

    fftw_plan forward_plan;
    fftw_plan inverse_plan;

    Simulation(double timeMax, double timeStep, particles initParticles, double width, int numOfCellsPerDim, double expanFac);
    ~Simulation();

    int cellIdentifier(std::vector<double> position); // used to identify the cell that a particle is in
    int wrapHelper(int i);                            // ensures that the index is within the box
    int indexCalculator(int i, int j, int k);         // used to calculate the index of a cell in the 1D array
    void kSquareUpdater();
    void densityCalculator();
    void potentialCalculator();
    void accelerationCalculator(fftw_complex *potential);
    void particlesUpdater(std::vector<std::vector<double>> acceleration);
    void boxExpander();

    /**
     * @brief Run a particle mesh simulation from t=0 to t_max
     *
     */
    void run(std::optional<std::string> folderPath = std::nullopt);
};