#include "Simulation.hpp"
#include "Utils.hpp"

Simulation::Simulation(double timeMax, double timeStep, particles initParticles, double width, int numOfCellsPerDim, double expanFac)
{
    this->timeMax = timeMax;
    this->timeStep = timeStep;
    this->particlesSimu = initParticles;
    this->width = width;
    this->numOfCellsPerDim = numOfCellsPerDim;
    this->expanFac = expanFac;

    this->numOfCells = pow(numOfCellsPerDim, 3);
    this->volOfBox = pow(this->width, 3);
    this->volOfCell = this->volOfBox / this->numOfCells;
    this->relCellWidth = 1.0 / this->numOfCellsPerDim;
    this->densityContributionPerParticle = particle::massGetter() / this->volOfCell; // need updating per iteration
    this->wSquare = this->width * this->width;
    this->kSquare = std::vector<double>(this->numOfCells);
    int index = 0;
    for (int i = 0; i < this->numOfCellsPerDim; i++)
    {
        for (int j = 0; j < this->numOfCellsPerDim; j++)
        {
            for (int k = 0; k < this->numOfCellsPerDim; k++)
            {
                index = i * pow(this->numOfCellsPerDim, 2) + j * this->numOfCellsPerDim + k;
                this->kSquare[index] = (k * k + j * j + i * i) / this->wSquare;
            }
        }
    }
    this->fNorm = 1.0 / (8 * this->numOfCells);

    this->densityBuffer = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * this->numOfCells);
    this->potentialBuffer = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * this->numOfCells);
    this->frequencyBuffer = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * this->numOfCells);

    this->forward_plan = fftw_plan_dft_3d(this->numOfCellsPerDim, this->numOfCellsPerDim, this->numOfCellsPerDim, this->densityBuffer, this->frequencyBuffer, FFTW_FORWARD, FFTW_MEASURE);
    this->inverse_plan = fftw_plan_dft_3d(this->numOfCellsPerDim, this->numOfCellsPerDim, this->numOfCellsPerDim, this->frequencyBuffer, this->potentialBuffer, FFTW_BACKWARD, FFTW_MEASURE);
}

Simulation::~Simulation()
{
    delete[] densityBuffer;
    densityBuffer = nullptr;
    delete[] potentialBuffer;
    potentialBuffer = nullptr;
    delete[] frequencyBuffer;
    frequencyBuffer = nullptr;

    fftw_destroy_plan(this->forward_plan);
    fftw_destroy_plan(this->inverse_plan);
}

void Simulation::densityCalculator()
{
    double normalizedPos = 0.0;
    double intePart = 0.0;
    int index = 0;

    memset(this->densityBuffer, 0.0, sizeof(fftw_complex) * this->numOfCells); // initialize the buffer with all entry to be 0.0

    for (auto iter = this->particlesSimu.particleInfo.begin(); iter < this->particlesSimu.particleInfo.end(); iter++)
    {
        index = 0;
        for (int i = 0; i < 3; ++i)
        {
            std::modf(iter->position[i] / this->relCellWidth, &intePart); // get the integer part from the division to get the index of each coordinates of the cell
            index += intePart * pow(this->numOfCellsPerDim, 2 - i);       // get the index of the cell in the 1-d array
        }
        this->densityBuffer[index][0] += this->densityContributionPerParticle; // add the density to the desity buffer representing the cell
    }
}

void Simulation::potentialCalculator()
{
    fftw_execute(this->forward_plan);

    this->frequencyBuffer[0][0] = 0.0;
    this->frequencyBuffer[0][1] = 0.0;
    for (int i = 1; i < this->numOfCells; i++)
    {
        this->frequencyBuffer[i][0] *= -4 * this->PI / kSquare[i] * this->fNorm;
        this->frequencyBuffer[i][1] *= -4 * this->PI / kSquare[i] * this->fNorm;
    }

    fftw_execute(this->inverse_plan);

    // for (int i = 0; i < this->numOfCells; i++)
    // {
    //     this->potentialBuffer[i][0] *= this->fNorm;
    //     this->potentialBuffer[i][1] *= this->fNorm;
    // }
}

void Simulation::run()
{
}