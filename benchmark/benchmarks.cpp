#include <iostream>
#include <string>
#include <omp.h>
#include <chrono>
#include "Simulation.hpp"

class BenchmarkData
{
public:
    BenchmarkData(std::string benchmark_name, int threads) : name(benchmark_name), num_threads(threads)
    {
        // set the number of threads
        omp_set_num_threads(threads);

// check whether the number of threads set successful
#pragma omp parallel
        {
            int threadID = omp_get_thread_num();
            if (threadID == 0)
            {
                int actualThreads = omp_get_num_threads();
                std::cout << "Actual number of threads have been setted as: " << actualThreads << std::endl;
            }
        }
    }

    double time;
    std::string name;
    int num_threads;
    // use for additional context such as number of particles / cells
    std::string info;

    void start()
    {
        t1 = std::chrono::high_resolution_clock::now();
    }

    void finish()
    {
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        time = (t2 - t1).count() / 1e9;
    }

    std::chrono::high_resolution_clock::time_point t1;
};

std::ostream &operator<<(std::ostream &os, const BenchmarkData &b)
{
    std::cout << "Benchmarking " << b.name << " with " << b.num_threads << " threads." << std::endl;
    std::cout << "Time = " << b.time << std::endl;
    std::cout << "Info: " << b.info << std::endl;
    return os;
}

int main()
{
    int numOfCellsPerDim = 101;
    int numOfParticles = 10000000;
    double timeMax = 1.5;
    double timeStep = 0.01;
    const int width = 100.0;
    double expanFac = 1.02;
    const int seed = 23093556;

    // // used for Simulation::kSquareUpdater optimization
    // BenchmarkData benchmarkData("Simulation::kSquareUpdater", 8);
    // Simulation simulation(timeMax, timeStep, particles(1, seed), width, 300, expanFac);
    // benchmarkData.start();
    // for (int i = 0; i < 50; i++)
    // {
    //     simulation.kSquareUpdater();
    // }
    // benchmarkData.finish();
    // benchmarkData.info = "collapse(2) schedule(dynamic), 300 cells wide the box";
    // std::cout << benchmarkData << std::endl;

    // Simulation::kSquareUpdater
    {
        // Simulation simulation1(timeMax, timeStep, particles(1, seed), width, 300, expanFac);
        for (int i = 0; i < 4; i++)
        {
            Simulation simulation = Simulation(timeMax, timeStep, particles(1, seed), width, 300, expanFac);
            BenchmarkData benchmarkData("Simulation::kSquareUpdater", static_cast<int>(pow(2, i)));
            benchmarkData.start();
            for (int i = 0; i < 50; i++)
            {
                simulation.kSquareUpdater();
            }
            benchmarkData.finish();
            benchmarkData.info = "#pragma omp parallel for collapse(2) schedule(dynamic), 300 cells wide the box";
            std::cout << benchmarkData << std::endl;
        }
    }

    // // used for Simulation::densityCalculator optimization
    // BenchmarkData benchmarkData("Simulation::densityCalculator", 8);
    // Simulation simulation(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
    // benchmarkData.start();
    // simulation.densityCalculator();
    // benchmarkData.finish();
    // benchmarkData.info = "parallelization C(dynamic), 10000000 numOfParticles with seed 23093556, 101 cells wide the box";
    // std::cout<<benchmarkData<<std::endl;

    // Simulation::densityCalculator
    {
        // Simulation simulation2(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
        for (int i = 0; i < 4; i++)
        {
            Simulation simulation = Simulation(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
            BenchmarkData benchmarkData("Simulation::densityCalculator", static_cast<int>(pow(2, i)));
            benchmarkData.start();
            simulation.densityCalculator();
            benchmarkData.finish();
            benchmarkData.info = "parallelization C(static), 10000000 numOfParticles with seed 23093556, 101 cells wide the box";
            std::cout << benchmarkData << std::endl;
        }
    }

    // // used for Simulation::potentialCalculator optimization
    // BenchmarkData benchmarkData("Simulation::potentialCalculator", 8);
    // Simulation simulation(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
    // simulation.densityCalculator();
    // benchmarkData.start();
    // for (int i = 0; i < 10; i++)
    // {
    //     simulation.potentialCalculator();
    // }
    // benchmarkData.finish();
    // benchmarkData.info = "schedule(dynamic), 10000000 numOfParticles with seed 23093556, 101 cells wide the box";
    // std::cout << benchmarkData << std::endl;

    // Simulation::potentialCalculator
    {
        // Simulation simulation3(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
        // simulation3.densityCalculator();
        for (int i = 0; i < 4; i++)
        {
            Simulation simulation = Simulation(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
            simulation.densityCalculator();

            BenchmarkData benchmarkData("Simulation::potentialCalculator", static_cast<int>(pow(2, i)));
            benchmarkData.start();
            for (int i = 0; i < 10; i++)
            {
                simulation.potentialCalculator();
            }
            benchmarkData.finish();
            benchmarkData.info = "#pragma omp parallel for schedule(guided), 10000000 numOfParticles with seed 23093556, 101 cells wide the box";
            std::cout << benchmarkData << std::endl;
        }
    }

    // // used for Simulation::accelerationCalculator optimization
    // BenchmarkData benchmarkData("Simulation::accelerationCalculator", 8);
    // Simulation simulation(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
    // simulation.densityCalculator();
    // simulation.potentialCalculator();
    // benchmarkData.start();
    // for (int i = 0; i < 100; i++)
    // {
    //     simulation.accelerationCalculator(simulation.potentialBuffer);
    // }
    // benchmarkData.finish();
    // benchmarkData.info = "collapse(2) schedule(dynamic), 10000000 numOfParticles with seed 23093556, 101 cells wide the box";
    // std::cout << benchmarkData << std::endl;

    // Simulation::accelerationCalculator
    {
        // Simulation simulation4(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
        // simulation4.densityCalculator();
        // simulation4.potentialCalculator();
        for (int i = 0; i < 4; i++)
        {
            Simulation simulation = Simulation(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
            simulation.densityCalculator();
            simulation.potentialCalculator();

            BenchmarkData benchmarkData("Simulation::accelerationCalculator", static_cast<int>(pow(2, i)));
            benchmarkData.start();
            for (int i = 0; i < 100; i++)
            {
                simulation.accelerationCalculator(simulation.potentialBuffer);
            }
            benchmarkData.finish();
            benchmarkData.info = "#pragma omp parallel for collapse(2) schedule(dynamic), 10000000 numOfParticles with seed 23093556, 101 cells wide the box";
            std::cout << benchmarkData << std::endl;
        }
    }

    // // used for Simulation::particlesUpdater optimization
    // BenchmarkData benchmarkData("Simulation::particlesUpdater", 8);
    // Simulation simulation(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
    // simulation.densityCalculator();
    // simulation.potentialCalculator();
    // simulation.accelerationCalculator(simulation.potentialBuffer);
    // benchmarkData.start();
    // simulation.particlesUpdater(simulation.acceleration);
    // benchmarkData.finish();
    // benchmarkData.info = "schedule(dynamic), 10000000 numOfParticles with seed 23093556, 101 cells wide the box";
    // std::cout << benchmarkData << std::endl;

    // Simulation::particlesUpdater
    {
        // Simulation simulation5(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
        // simulation5.densityCalculator();
        // simulation5.potentialCalculator();
        // simulation5.accelerationCalculator(simulation5.potentialBuffer);
        for (int i = 0; i < 4; i++)
        {
            Simulation simulation = Simulation(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
            simulation.densityCalculator();
            simulation.potentialCalculator();
            simulation.accelerationCalculator(simulation.potentialBuffer);

            BenchmarkData benchmarkData("Simulation::particlesUpdater", static_cast<int>(pow(2, i)));
            benchmarkData.start();
            simulation.particlesUpdater(simulation.acceleration);
            benchmarkData.finish();
            benchmarkData.info = "#pragma omp parallel for schedule(static), 10000000 numOfParticles with seed 23093556, 101 cells wide the box";
            std::cout << benchmarkData << std::endl;
        }
    }

    // // used for Simulation::boxExpander optimization
    // BenchmarkData benchmarkData("Simulation::boxExpander", 8);
    // Simulation simulation(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
    // simulation.densityCalculator();
    // simulation.potentialCalculator();
    // simulation.accelerationCalculator(simulation.potentialBuffer);
    // simulation.particlesUpdater(simulation.acceleration);
    // benchmarkData.start();
    // for (int i = 0; i < 10; i++)
    // {
    //     simulation.boxExpander();
    // }
    // benchmarkData.finish();
    // benchmarkData.info = "schedule(dynamic), 10000000 numOfParticles with seed 23093556, 101 cells wide the box";
    // std::cout << benchmarkData << std::endl;

    // Simulation::boxExpander
    {
        Simulation simulation6(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
        simulation6.densityCalculator();
        simulation6.potentialCalculator();
        simulation6.accelerationCalculator(simulation6.potentialBuffer);
        simulation6.particlesUpdater(simulation6.acceleration);
        for (int i = 0; i < 4; i++)
        {
            Simulation simulation = Simulation(timeMax, timeStep, particles(numOfParticles, seed), width, numOfCellsPerDim, expanFac);
            simulation.densityCalculator();
            simulation.potentialCalculator();
            simulation.accelerationCalculator(simulation.potentialBuffer);
            simulation.particlesUpdater(simulation.acceleration);

            BenchmarkData benchmarkData("Simulation::boxExpander", static_cast<int>(pow(2, i)));
            benchmarkData.start();
            for (int i = 0; i < 10; i++)
            {
                simulation.boxExpander();
            }
            benchmarkData.finish();
            benchmarkData.info = "schedule(guided), 10000000 numOfParticles with seed 23093556, 101 cells wide the box";
            std::cout << benchmarkData << std::endl;
        }
    }

    return 0;
}