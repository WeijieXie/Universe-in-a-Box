#pragma once

#include "particles.hpp"
#include "Utils.hpp"

#include <fftw3.h>
#include <cstring>
#include <optional>
#include <iostream>

#include <filesystem>
#include <sstream>

class Simulation
{
public:
    double timeMax;
    double timeStep;
    double width;
    int numOfCellsPerDim;
    double expanFac;

    int numOfCells;
    double volOfBox;
    double volOfCell;
    double relCellWidth; // which is 1.0/numOfCellsPerDim
    double cellWidth;
    double densityContributionPerParticle;
    double wSquare;
    std::vector<double> kSquare;
    const double PI = 3.141592653589793238463;
    double fNorm;

    particles particlesSimu;

    fftw_complex *densityBuffer = nullptr;
    fftw_complex *potentialBuffer = nullptr;
    fftw_complex *frequencyBuffer = nullptr;
    std::vector<double> potentialRealPart;
    std::vector<std::vector<double>> acceleration;

    fftw_plan forward_plan;
    fftw_plan inverse_plan;

    Simulation(double timeMax, double timeStep, particles initParticles, double width, int numOfCellsPerDim, double expanFac);
    ~Simulation();

    int cellIdentifier(std::vector<double> position);
    int wrapHelper(int i);
    int indexCalculator(int i, int j, int k);
    void densityCalculator();
    void potentialCalculator();
    void accelerationCalculator(std::vector<double> potentialRealPart);
    void particlesUpdater(std::vector<std::vector<double>> acceleration);
    void boxExpander();

    /**
     * @brief Run a particle mesh simulation from t=0 to t_max
     *
     */
    void run(std::optional<std::string> folderPath = std::nullopt);
};