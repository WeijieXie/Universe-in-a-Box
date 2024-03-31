#include "Simulation.hpp"

Simulation::Simulation(double timeMax, double timeStep, particles initParticles, double width, int numOfCellsPerDim, double expanFac)
{
    this->timeMax = timeMax;
    this->timeStep = timeStep;
    particle::dtSetter(this->timeStep);
    this->particlesSimu = initParticles;
    this->width = width;
    this->numOfCellsPerDim = numOfCellsPerDim;
    this->expanFac = expanFac;

    this->numOfCells = pow(numOfCellsPerDim, 3);
    this->numOfParticles = this->particlesSimu.particleInfo.size();
    this->volOfBox = pow(this->width, 3);
    this->volOfCell = this->volOfBox / this->numOfCells;
    this->relCellWidth = 1.0 / this->numOfCellsPerDim;
    this->cellWidth = this->width / this->numOfCellsPerDim;
    this->densityContributionPerParticle = particle::massGetter() / this->volOfCell; // need updating per iteration
    this->wSquare = this->width * this->width;
    this->kSquare = std::vector<double>(this->numOfCells);
    kSquareUpdater();
    this->fNorm = 1.0 / (8 * this->numOfCells);

    this->densityBuffer = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * this->numOfCells);
    this->potentialBuffer = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * this->numOfCells);
    this->frequencyBuffer = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * this->numOfCells);
    this->acceleration = std::vector<std::vector<double>>(this->numOfCells, std::vector<double>(3));

    this->forward_plan = fftw_plan_dft_3d(this->numOfCellsPerDim, this->numOfCellsPerDim, this->numOfCellsPerDim, this->densityBuffer, this->frequencyBuffer, FFTW_FORWARD, FFTW_MEASURE);
    this->inverse_plan = fftw_plan_dft_3d(this->numOfCellsPerDim, this->numOfCellsPerDim, this->numOfCellsPerDim, this->frequencyBuffer, this->potentialBuffer, FFTW_BACKWARD, FFTW_MEASURE);
}

Simulation::~Simulation()
{
    fftw_free(this->densityBuffer);
    densityBuffer = nullptr;
    fftw_free(this->potentialBuffer);
    potentialBuffer = nullptr;
    fftw_free(this->frequencyBuffer);
    frequencyBuffer = nullptr;

    fftw_destroy_plan(this->forward_plan);
    fftw_destroy_plan(this->inverse_plan);
}

int Simulation::cellIdentifier(std::vector<double> position)
{
    double quotient = 0.0;
    int index = 0;
    for (int i = 0; i < 3; ++i)
    {
        quotient = std::floor(position[i] / this->relCellWidth); // get the integer part from the division to get the index of each coordinates of the cell
        index += quotient * pow(this->numOfCellsPerDim, 2 - i);  // get the index of the cell in the 1-d array
    }
    return static_cast<int>(index);
}

int Simulation::wrapHelper(int i)
{
    if (i == this->numOfCellsPerDim)

    {
        return 0;
    }
    else if (i == -1)
    {
        return this->numOfCellsPerDim - 1;
    }
    else
    {
        return i;
    }
}

int Simulation::indexCalculator(int i, int j, int k)
{
    int index = i * pow(this->numOfCellsPerDim, 2) + j * this->numOfCellsPerDim + k;
    return index;
}

void Simulation::kSquareUpdater()
{
    // #pragma omp parallel for collapse(3)
    // #pragma omp parallel for collapse(2)
    // #pragma omp parallel for collapse(2) schedule(static)
    // #pragma omp parallel for collapse(2) schedule(guided)
    // #pragma omp parallel for
#pragma omp parallel for collapse(2) schedule(dynamic)
    for (int i = 0; i < this->numOfCellsPerDim; i++)
    {
        for (int j = 0; j < this->numOfCellsPerDim; j++)
        {
            for (int k = 0; k < this->numOfCellsPerDim; k++)
            {
                int index = indexCalculator(i, j, k);
                // bool is_thread_even = (omp_get_thread_num() % 2 == 0);
                // if (is_thread_even)
                // {
                //     std::this_thread::sleep_for(std::chrono::microseconds(1));
                // }
                this->kSquare[index] = (k * k + j * j + i * i) / this->wSquare;
            }
        }
    }
}

void Simulation::densityCalculator()
{
    // // without parallelization
    // memset(this->densityBuffer, 0.0, sizeof(fftw_complex) * this->numOfCells); // initialize the buffer with all entry to be 0.0
    // int index = 0;
    // for (int i = 0; i < this->numOfParticles; ++i)
    // {
    //     index = cellIdentifier(this->particlesSimu.particleInfo[i].position);
    //     this->densityBuffer[index][0] += this->densityContributionPerParticle; // add the density to the desity buffer representing the cell
    // }

    //     // parallelization A
    //     memset(this->densityBuffer, 0.0, sizeof(fftw_complex) * this->numOfCells); // initialize the buffer with all entry to be 0.0
    // #pragma omp parallel for
    //     for (int i = 0; i < this->numOfParticles; ++i)
    //     {
    //         int index = cellIdentifier(this->particlesSimu.particleInfo[i].position);
    // #pragma omp critical
    //         this->densityBuffer[index][0] += this->densityContributionPerParticle; // add the density to the desity buffer representing the cell
    //     }

    //     // parallelization B
    //     memset(densityBuffer, 0.0, sizeof(fftw_complex) * this->numOfCells); // initialize the buffer with all entry to be 0.0
    // #pragma omp parallel for reduction(+ : densityBuffer[index][0])
    //     for (int i = 0; i < this->numOfParticles; ++i)
    //     {
    //         int index = cellIdentifier(this->particlesSimu.particleInfo[i].position);
    // #pragma omp critical
    //         densityBuffer[index][0] += this->densityContributionPerParticle; // add the density to the desity buffer representing the cell
    //     }

    //     // parallelization C
    //     memset(this->densityBuffer, 0.0, sizeof(fftw_complex) * this->numOfCells); // initialize the buffer with all entry to be 0.0
    // #pragma omp parallel for
    //     for (int i = 0; i < this->numOfParticles; ++i)
    //     {
    //         int index = cellIdentifier(this->particlesSimu.particleInfo[i].position);
    // #pragma omp atomic
    //         this->densityBuffer[index][0] += this->densityContributionPerParticle; // add the density to the desity buffer representing the cell
    //     }

    //     // parallelization D
    //     memset(this->densityBuffer, 0.0, sizeof(fftw_complex) * this->numOfCells); // initialize the buffer with all entry to be 0.0
    // #pragma omp parallel
    //     {
    //         std::vector<double> localDensity(this->numOfCells, 0.0);
    // #pragma omp for nowait
    //         for (int i = 0; i < this->numOfParticles; ++i)
    //         {
    //             int index = cellIdentifier(this->particlesSimu.particleInfo[i].position);
    //             localDensity[index] += this->densityContributionPerParticle; // add the density to the desity buffer representing the cell
    //         }
    // #pragma omp critical
    //         for (int i = 0; i < this->numOfCells; ++i)
    //         {
    //             this->densityBuffer[i][0] += localDensity[i];
    //         }
    //     }

    //     // parallelization E
    //     memset(this->densityBuffer, 0.0, sizeof(fftw_complex) * this->numOfCells); // initialize the buffer with all entry to be 0.0
    // #pragma omp parallel
    //     {
    //         std::vector<double> localDensity(this->numOfCells, 0.0);
    // #pragma omp for nowait
    //         for (int i = 0; i < this->numOfParticles; ++i)
    //         {
    //             int index = cellIdentifier(this->particlesSimu.particleInfo[i].position);
    //             localDensity[index] += this->densityContributionPerParticle; // add the density to the desity buffer representing the cell
    //         }
    //         for (int i = 0; i < this->numOfCells; ++i)
    //         {
    // #pragma omp atomic
    //             this->densityBuffer[i][0] += localDensity[i];
    //         }
    //     }

    // parallelization C(static)
    memset(this->densityBuffer, 0.0, sizeof(fftw_complex) * this->numOfCells); // initialize the buffer with all entry to be 0.0
#pragma omp parallel for schedule(static)
    for (int i = 0; i < this->numOfParticles; ++i)
    {
        int index = cellIdentifier(this->particlesSimu.particleInfo[i].position);
#pragma omp atomic
        this->densityBuffer[index][0] += this->densityContributionPerParticle; // add the density to the desity buffer representing the cell
    }

    //     // parallelization C(guided)
    //     memset(this->densityBuffer, 0.0, sizeof(fftw_complex) * this->numOfCells); // initialize the buffer with all entry to be 0.0
    // #pragma omp parallel for schedule(guided)
    //     for (int i = 0; i < this->numOfParticles; ++i)
    //     {
    //         int index = cellIdentifier(this->particlesSimu.particleInfo[i].position);
    // #pragma omp atomic
    //         this->densityBuffer[index][0] += this->densityContributionPerParticle; // add the density to the desity buffer representing the cell
    //     }

    //         // parallelization C(dynamic)
    //     memset(this->densityBuffer, 0.0, sizeof(fftw_complex) * this->numOfCells); // initialize the buffer with all entry to be 0.0
    // #pragma omp parallel for schedule(dynamic)
    //     for (int i = 0; i < this->numOfParticles; ++i)
    //     {
    //         int index = cellIdentifier(this->particlesSimu.particleInfo[i].position);
    // #pragma omp atomic
    //         this->densityBuffer[index][0] += this->densityContributionPerParticle; // add the density to the desity buffer representing the cell
    //     }
}

void Simulation::potentialCalculator()
{
    fftw_execute(this->forward_plan);

    this->frequencyBuffer[0][0] = 0.0;
    this->frequencyBuffer[0][1] = 0.0;
// #pragma omp parallel for
// #pragma omp parallel for schedule(static)
// #pragma omp parallel for schedule(dynamic)
#pragma omp parallel for schedule(guided)
    for (int i = 1; i < this->numOfCells; i++)
    {
        this->frequencyBuffer[i][0] *= -4 * this->PI / kSquare[i] * this->fNorm;
        this->frequencyBuffer[i][1] *= -4 * this->PI / kSquare[i] * this->fNorm;
    }

    fftw_execute(this->inverse_plan);
}

void Simulation::accelerationCalculator(fftw_complex *potential)
{
// #pragma omp parallel for collapse(3)
// #pragma omp parallel for collapse(2)
// #pragma omp parallel for
// #pragma omp parallel for collapse(2) schedule(static)
// #pragma omp parallel for collapse(2) schedule(guided)
#pragma omp parallel for collapse(2) schedule(dynamic)
    for (int i = 0; i < this->numOfCellsPerDim; i++)
    {
        for (int j = 0; j < this->numOfCellsPerDim; j++)
        {
            for (int k = 0; k < this->numOfCellsPerDim; k++)
            {
                int index = indexCalculator(i, j, k);

                this->acceleration[index][0] = (potential[indexCalculator(wrapHelper(i - 1), j, k)][0] - potential[indexCalculator(wrapHelper(i + 1), j, k)][0]) / (2.0 * this->cellWidth);
                this->acceleration[index][1] = (potential[indexCalculator(i, wrapHelper(j - 1), k)][0] - potential[indexCalculator(i, wrapHelper(j + 1), k)][0]) / (2.0 * this->cellWidth);
                this->acceleration[index][2] = (potential[indexCalculator(i, j, wrapHelper(k - 1))][0] - potential[indexCalculator(i, j, wrapHelper(k + 1))][0]) / (2.0 * this->cellWidth);
            }
        }
    }
}

void Simulation::particlesUpdater(std::vector<std::vector<double>> acceleration)
{
// #pragma omp parallel for
// #pragma omp parallel for schedule(guided)
// #pragma omp parallel for schedule(dynamic)
#pragma omp parallel for schedule(static)
    for (int i = 0; i < this->numOfParticles; ++i)
    {
        int index = cellIdentifier(this->particlesSimu.particleInfo[i].position);
        this->particlesSimu.particleInfo[i].updater(acceleration[index]);
    }
}

void Simulation::boxExpander()
{
    this->width *= this->expanFac;
    this->volOfBox = pow(this->width, 3);
    this->volOfCell = this->volOfBox / this->numOfCells;
    this->cellWidth = this->width / this->numOfCellsPerDim;
    this->densityContributionPerParticle = particle::massGetter() / this->volOfCell;
    this->wSquare = this->width * this->width;
    kSquareUpdater();
// #pragma omp parallel for
// #pragma omp parallel for schedule(static)
// #pragma omp parallel for schedule(dynamic)
#pragma omp parallel for schedule(guided)
    for (int i = 0; i < this->numOfParticles; ++i)
    {
        this->particlesSimu.particleInfo[i].velocityRescaler(this->expanFac);
    }
}

void Simulation::run(std::optional<std::string> folderPath)
{
    bool flag = false;
    int numSteps = static_cast<int>(std::floor(this->timeMax / this->timeStep));

    if (folderPath.has_value())
    {
        flag = true;
        if (!std::filesystem::exists("Images"))
        {
            std::filesystem::create_directories("Images");
        }
        if (!std::filesystem::exists(std::string("Images") + "/" + (*folderPath)))
        {
            std::filesystem::create_directories(std::string("Images") + "/" + (*folderPath));
        }
    }

    for (int i = 0; i <= numSteps; i++)
    {
        densityCalculator();
        if (i % 10 == 0)
        {
            if (flag == true)
            {
                std::string full_path = std::string("Images") + "/" + (*folderPath) + "/" + "iteration" + std::to_string(i) + ".pbm";
                SaveToFile(this->densityBuffer, this->numOfCellsPerDim, full_path);
            }
        }
        potentialCalculator();
        accelerationCalculator(this->potentialBuffer);
        particlesUpdater(this->acceleration);
        boxExpander();
    }
}