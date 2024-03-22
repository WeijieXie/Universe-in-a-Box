#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <cmath>
#include<iostream>
#include "data.hpp"

using namespace Catch::Matchers;

TEST_CASE("Test the imaginary part of density to be 0.0", "Density Tests")
{
    //One particle in the centre of the box with mass 0.01
    particle::massSetter(test_data::mass);

    // These properties should apply to your box
    int numOfCellsPerDim = 101;

    //Declare simulation with particle setup, width, and number of cells. 
    //TODO: Simulation sim(...);
    Simulation simulation(test_data::timeMax, test_data::timeStep, test_data::initParticlesRandom, test_data::width, numOfCellsPerDim, test_data::expanFac);

    //TODO: calculate density
    simulation.densityCalculator();

    int size = 0;
    for (auto iter = simulation.densityBuffer; iter < simulation.densityBuffer + simulation.numOfCells; iter++)
    {
        size ++;
    }
    REQUIRE(size==1030301);
    // std::cout<<"size: "<<size<<std::endl;

    // Look at the density buffer
    for (auto iter = simulation.densityBuffer; iter < simulation.densityBuffer + simulation.numOfCells; iter++)
    {
        REQUIRE((*iter)[1]==0.0);
    }
}

TEST_CASE("Test density of an empty space", "Density Tests")
{
    //One particle in the centre of the box with mass 0.01
    particle::massSetter(test_data::mass);

    // These properties should apply to your box
    int numOfCellsPerDim = 101;

    //Declare simulation with particle setup, width, and number of cells. 
    //TODO: Simulation sim(...);
    Simulation simulation(test_data::timeMax, test_data::timeStep, test_data::initParticlesEmpty, test_data::width, numOfCellsPerDim, test_data::expanFac);

    //TODO: calculate density
    simulation.densityCalculator();

    int size = 0;
    for (auto iter = simulation.densityBuffer; iter < simulation.densityBuffer + simulation.numOfCells; iter++)
    {
        size ++;
    }
    REQUIRE(size==1030301);
    // std::cout<<"size: "<<size<<std::endl;

    // Look at the density buffer
    for (auto iter = simulation.densityBuffer; iter < simulation.densityBuffer + simulation.numOfCells; iter++)
    {
        REQUIRE((*iter)[0]==0.0);
        REQUIRE((*iter)[1]==0.0);
    }
}

TEST_CASE("Test density of single particle", "Density Tests")
{
    //One particle in the centre of the box with mass 0.01
    particle::massSetter(test_data::mass);

    //Declare simulation with particle setup, width, and number of cells. 
    //TODO: Simulation sim(...);
    Simulation simulation(test_data::timeMax, test_data::timeStep, test_data::initParticlesOneInMid, test_data::width, test_data::numOfCellsPerDim, test_data::expanFac);

    //TODO: calculate density
    simulation.densityCalculator();

    int size = 0;
    for (auto iter = simulation.densityBuffer; iter < simulation.densityBuffer + simulation.numOfCells; iter++)
    {
        size ++;
    }
    REQUIRE(size==8);
    // std::cout<<"size: "<<size<<std::endl;

    // Look at the density buffer
    for (int i=0; i<simulation.numOfCells; i++)
    {
        // std::cout<<i<<": "<< simulation.densityBuffer[i][0]<<", "<< simulation.densityBuffer[i][1]<<std::endl;;
        REQUIRE(simulation.densityBuffer[i][0]==test_data::densityBufferForSingleParticle[i][0]);
        REQUIRE(simulation.densityBuffer[i][1]==0.0);
    }
}

TEST_CASE("Test density of multiple particles", "Density Tests")
{
    //One particle in the centre of the box with mass 0.01
    particle::massSetter(test_data::mass);

    //Declare simulation with particle setup, width, and number of cells. 
    //TODO: Simulation sim(...);
    Simulation simulation(test_data::timeMax, test_data::timeStep, test_data::initParticlesMultiple, test_data::width, test_data::numOfCellsPerDim, test_data::expanFac);

    //TODO: calculate density
    simulation.densityCalculator();

    int size = 0;
    for (auto iter = simulation.densityBuffer; iter < simulation.densityBuffer + simulation.numOfCells; iter++)
    {
        size ++;
    }
    REQUIRE(size==8);
    // std::cout<<"size: "<<size<<std::endl;

    // Look at the density buffer
    for (int i=0; i<simulation.numOfCells; i++)
    {
        // std::cout<<i<<": "<< simulation.densityBuffer[i][0]<<", "<< simulation.densityBuffer[i][1]<<std::endl;;
        REQUIRE(simulation.densityBuffer[i][0]==test_data::densityBufferForMultipleParticles[i][0]);
        REQUIRE(simulation.densityBuffer[i][1]==0.0);
    }
}
/**
 * @brief Fill out this test function by filling in the TODOs
 * Tests the calculation of the gravitational potential due to a single particle
 * Potential is examined along the x axis throught the centre of the cube
 */
// TEST_CASE("Test potential function for single particle", "Potential Tests")
// {
//     //One particle in the centre of the box with mass 0.01
//     //TODO: Set up single particle with position {0.5, 0.5, 0.5}
//     double mass = 0.01;

//     // These properties should apply to your box
//     double width = 100;
//     int ncells = 101;

//     //Declare simulation with particle setup, width, and number of cells. 
//     //TODO: Simulation sim(...);
//     //TODO: calculate density
//     //TODO: calculate potential

//     double w_c = width / ncells;
//     // Look at the potential function along the x axis
//     for (int i = 0; i < 101; i++)
//     {
//         int j = 50;
//         int k = 50;
//         if (i == 50 && j == 50 && k == 50)
//         {
//             //ignore the centre where point potential would be singular
//             continue;
//         }
//         else
//         {
//             // ignore imaginary component
//             // approximate potential for multiple sources due to periodicity
//             double dx1 = std::abs(i - 50) * w_c;
//             double dx2 = std::abs(i - 151) * w_c;
//             double dx3 = std::abs(i + 51) * w_c;
//             double pot; //TODO = your potential function at indices (i,j,k)
//             double expected_pot =  -mass *(1/ dx1 + 1/dx2 + 1/dx3);
//             double diff = pot - expected_pot;
//             REQUIRE_THAT(pot, WithinRel(expected_pot, 0.3));
//         }
//     }
// }