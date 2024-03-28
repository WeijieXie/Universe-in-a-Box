#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <cmath>
#include "data.hpp"

using namespace Catch::Matchers;

TEST_CASE("Test simulation constructor", "Constructor Tests")
{
    // Declare simulation with particle setup, width, and number of cells.
    // TODO: Simulation sim(...);

    int numOfCellsPerDim = 101;
    int numOfParticlesPerCell = 10;
    double timeMax = 1.5;
    double timeStep = 0.01;
    double expanFac = 1.0;
    std::optional<std::string> outputFolder = std::nullopt;
    int seed = 23093556;

    int numOfParticles = pow(numOfCellsPerDim, 3) * numOfParticlesPerCell;
    particles initParticles = particles(numOfParticles, seed);
    particle::massSetter(100000.0 / numOfParticlesPerCell);
    Simulation simulation(timeMax, timeStep, initParticles, 100.0, numOfCellsPerDim, expanFac);

    REQUIRE(simulation.particlesSimu.particleInfo.size() == numOfParticles);
}

TEST_CASE("Test the imaginary part of density to be 0.0", "Density Tests")
{
    // One particle in the centre of the box with mass 0.01
    particle::massSetter(test_data::mass);

    // These properties should apply to your box
    int numOfCellsPerDim = 101;

    // Declare simulation with particle setup, width, and number of cells.
    // TODO: Simulation sim(...);
    Simulation simulation(test_data::timeMax, test_data::timeStep, test_data::initParticlesRandom, test_data::width, numOfCellsPerDim, test_data::expanFac);

    // TODO: calculate density
    simulation.densityCalculator();

    int size = 0;
    for (auto iter = simulation.densityBuffer; iter < simulation.densityBuffer + simulation.numOfCells; iter++)
    {
        size++;
    }
    REQUIRE(size == 1030301);

    // Look at the density buffer
    for (auto iter = simulation.densityBuffer; iter < simulation.densityBuffer + simulation.numOfCells; iter++)
    {
        REQUIRE((*iter)[1] == 0.0);
    }
}

TEST_CASE("Test density of an empty space", "Density Tests")
{
    // One particle in the centre of the box with mass 0.01
    particle::massSetter(test_data::mass);

    // These properties should apply to your box
    int numOfCellsPerDim = 101;

    // Declare simulation with particle setup, width, and number of cells.
    // TODO: Simulation sim(...);
    Simulation simulation(test_data::timeMax, test_data::timeStep, test_data::initParticlesEmpty, test_data::width, numOfCellsPerDim, test_data::expanFac);

    // TODO: calculate density
    simulation.densityCalculator();

    int size = 0;
    for (auto iter = simulation.densityBuffer; iter < simulation.densityBuffer + simulation.numOfCells; iter++)
    {
        size++;
    }
    REQUIRE(size == 1030301);
    // std::cout<<"size: "<<size<<std::endl;

    // Look at the density buffer
    for (auto iter = simulation.densityBuffer; iter < simulation.densityBuffer + simulation.numOfCells; iter++)
    {
        REQUIRE((*iter)[0] == 0.0);
        REQUIRE((*iter)[1] == 0.0);
    }
}

TEST_CASE("Test density of single particle", "Density Tests")
{
    // One particle in the centre of the box with mass 0.01
    particle::massSetter(test_data::mass);

    // Declare simulation with particle setup, width, and number of cells.
    // TODO: Simulation sim(...);
    Simulation simulation(test_data::timeMax, test_data::timeStep, test_data::initParticlesOneInMid, test_data::width, test_data::numOfCellsPerDim, test_data::expanFac);

    // TODO: calculate density
    simulation.densityCalculator();

    int size = 0;
    for (auto iter = simulation.densityBuffer; iter < simulation.densityBuffer + simulation.numOfCells; iter++)
    {
        size++;
    }
    REQUIRE(size == 8);

    // Look at the density buffer
    for (int i = 0; i < simulation.numOfCells; i++)
    {
        REQUIRE(simulation.densityBuffer[i][0] == test_data::densityBufferForSingleParticle[i][0]);
        REQUIRE(simulation.densityBuffer[i][1] == 0.0);
    }
}

TEST_CASE("Test density of multiple particles", "Density Tests")
{
    // One particle in the centre of the box with mass 0.01
    particle::massSetter(test_data::mass);

    // Declare simulation with particle setup, width, and number of cells.
    // TODO: Simulation sim(...);
    Simulation simulation(test_data::timeMax, test_data::timeStep, test_data::initParticlesMultiple, test_data::width, test_data::numOfCellsPerDim, test_data::expanFac);

    // TODO: calculate density
    simulation.densityCalculator();

    int size = 0;
    for (auto iter = simulation.densityBuffer; iter < simulation.densityBuffer + simulation.numOfCells; iter++)
    {
        size++;
    }
    REQUIRE(size == 8);

    // Look at the density buffer
    for (int i = 0; i < simulation.numOfCells; i++)
    {
        REQUIRE(simulation.densityBuffer[i][0] == test_data::densityBufferForMultipleParticles[i][0]);
        REQUIRE(simulation.densityBuffer[i][1] == 0.0);
    }
}

/**
 * @brief Fill out this test function by filling in the TODOs
 * Tests the calculation of the gravitational potential due to a single particle
 * Potential is examined along the x axis throught the centre of the cube
 */
TEST_CASE("Test potential function for single particle", "Potential Tests")
{
    // One particle in the centre of the box with mass 0.01
    // TODO: Set up single particle with position {0.5, 0.5, 0.5}
    double mass = 0.01;
    particle::massSetter(mass);

    // These properties should apply to your box
    double width = 100;
    int ncells = 101;

    // Declare simulation with particle setup, width, and number of cells.
    // TODO: Simulation sim(...);
    Simulation simulation(test_data::timeMax, test_data::timeStep, test_data::initParticlesOneInMid, width, ncells, test_data::expanFac);
    // TODO: calculate density
    simulation.densityCalculator();
    // TODO: calculate potential
    simulation.potentialCalculator();

    double w_c = width / ncells;
    // Look at the potential function along the x axis
    for (int i = 0; i < 101; i++)
    {
        int j = 50;
        int k = 50;
        if (i == 50 && j == 50 && k == 50)
        {
            // ignore the centre where point potential would be singular
            continue;
        }
        else
        {
            // ignore imaginary component
            // approximate potential for multiple sources due to periodicity
            double dx1 = std::abs(i - 50) * w_c;
            double dx2 = std::abs(i - 151) * w_c;
            double dx3 = std::abs(i + 51) * w_c;
            double pot; // TODO = your potential function at indices (i,j,k)
            int index = i * pow(ncells, 2) + j * ncells + k;
            pot = simulation.potentialBuffer[index][0];
            double expected_pot = -mass * (1 / dx1 + 1 / dx2 + 1 / dx3);
            double diff = pot - expected_pot;
            // std::cout<<"i: "<<i<<" exp: "<<expected_pot<<" act: "<<pot<<" diff: "<<diff<<std::endl;
            REQUIRE_THAT(pot, WithinRel(expected_pot, 0.3));
        }
    }
}

TEST_CASE("Test the gradient in a cube with 2*2*2 cells", "Gradient Tests")
{
    // One particle in the centre of the box with mass 0.01
    particle::massSetter(test_data::mass);

    // Declare simulation with particle setup, width, and number of cells.
    // TODO: Simulation sim(...);
    Simulation simulation(test_data::timeMax, test_data::timeStep, test_data::initParticlesMultiple, test_data::width, test_data::numOfCellsPerDim, test_data::expanFac);
    simulation.accelerationCalculator(simulation.potentialRealPart);
    REQUIRE(simulation.acceleration == test_data::zeroGradient); // in a 2*2*2 cube, the gradient should be 0 everywhere because of wrapping around
}

TEST_CASE("Test the gradient in a cube with 3*3*3 cells", "Gradient Tests")
{
    // One particle in the centre of the box with mass 0.01
    particle::massSetter(test_data::mass);

    // Declare simulation with particle setup, width, and number of cells.
    // TODO: Simulation sim(...);
    Simulation simulation(test_data::timeMax, test_data::timeStep, test_data::initParticlesMultiple, test_data::width, 3, test_data::expanFac);
    simulation.accelerationCalculator(test_data::potentialRealPart_27);
    REQUIRE(simulation.acceleration == test_data::gradient_27);
}

TEST_CASE("Test the updater in a cube with 2*2*2 cells", "Updater Tests")
{
    // One particle in the centre of the box with mass 0.01
    particle::massSetter(test_data::mass);

    // Declare simulation with particle setup, width, and number of cells.
    // TODO: Simulation sim(...);
    Simulation simulation(test_data::timeMax, test_data::timeStep, test_data::initParticlesAB, test_data::width, test_data::numOfCellsPerDim, test_data::expanFac);
    simulation.particlesUpdater(test_data::acceleration);
    simulation.particlesUpdater(test_data::acceleration);
    for (int i = 0; i < 3; i++)
    {
        REQUIRE_THAT(simulation.particlesSimu.particleInfo[0].position[i], WithinRel(test_data::partilceAPos[i], 0.001));
        REQUIRE_THAT(simulation.particlesSimu.particleInfo[0].velocity[i], WithinRel(test_data::partilceASpeed[i], 0.001));
        REQUIRE_THAT(simulation.particlesSimu.particleInfo[1].position[i], WithinRel(test_data::partilceBPos[i], 0.001));
        REQUIRE_THAT(simulation.particlesSimu.particleInfo[1].velocity[i], WithinRel(test_data::partilceBSpeed[i], 0.001));
    }
}

TEST_CASE("Test simulation run", "Simulation Tests")
{
    // One particle in the centre of the box with mass 0.01
    particle::massSetter(test_data::mass);

    // Declare simulation with particle setup, width, and number of cells.
    // TODO: Simulation sim(...);
    Simulation simulation(test_data::timeMax, test_data::timeStep, test_data::initParticlesMultiple, test_data::width, 3, test_data::expanFac);

    simulation.run();
}