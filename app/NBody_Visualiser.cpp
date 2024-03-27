#include "Simulation.hpp"
#include "data.hpp"

#include <iostream>
#include <string>
#include <cstdlib>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cout << "No arguments provided. Use -h for help." << std::endl;
        return 1;
    }

    int numOfCellsPerDim = 101;
    int numOfParticlesPerCell = 10;
    double timeMax = 1.5;
    double timeStep = 0.01;
    double expanFac = 1.0;
    std::optional<std::string> outputFolder = std::nullopt;
    int seed = 23093556;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-h")
        {
            std::cout << "Usage: NBody_Visualiser [OPTIONS]\n"
                      << "Options:\n"
                      << "  -nc <value>    The number of cells wide the box is\n"
                      << "  -np <value>    The average number of particles per cell\n"
                      << "  -t <value>     The total time\n"
                      << "  -dt <value>    The time step\n"
                      << "  -F <value>     The expansion factor\n"
                      << "  -o <folder>    The output folder\n"
                      << "  -s <value>     The random seed\n"
                      << "  -h             Display this help message\n";
            return 0; // Exit after displaying help
        }
        else if (arg == "-nc" && i + 1 < argc)
        {
            numOfCellsPerDim = std::atoi(argv[++i]);
        }
        else if (arg == "-np" && i + 1 < argc)
        {
            numOfParticlesPerCell = std::atoi(argv[++i]);
        }
        else if (arg == "-t" && i + 1 < argc)
        {
            timeMax = std::atof(argv[++i]);
        }
        else if (arg == "-dt" && i + 1 < argc)
        {
            timeStep = std::atof(argv[++i]);
        }
        else if (arg == "-F" && i + 1 < argc)
        {
            expanFac = std::atof(argv[++i]);
        }
        else if (arg == "-o" && i + 1 < argc)
        {
            outputFolder = argv[++i];
        }
        else if (arg == "-s" && i + 1 < argc)
        {
            seed = std::atoi(argv[++i]);
        }
        else
        {
            std::cerr << "Unknown or incomplete argument: " << arg << std::endl;
            return 1;
        }
    }

    int numOfParticles = pow(numOfCellsPerDim, 3) * numOfParticlesPerCell;

    // particles initParticlesTest = particles({{0.5, 0.5, 0.5}, {0.999, 0.999, 0.999}, {0.999, 0.999, 0.999}});
    // particles initParticles = initParticlesTest;

    particles initParticles = particles(numOfParticles, seed);
    particle::massSetter(100000.0 / numOfParticlesPerCell);
    Simulation simulation(timeMax, timeStep, initParticles, 100.0, numOfCellsPerDim, expanFac);
    simulation.run(outputFolder);

    return 0;
}
