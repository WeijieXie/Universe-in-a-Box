#include "Simulation.hpp"
#include "Utils.hpp"

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
    this->volOfBox = pow(this->width, 3);
    this->volOfCell = this->volOfBox / this->numOfCells;
    this->relCellWidth = 1.0 / this->numOfCellsPerDim;
    this->cellWidth = this->width / this->numOfCellsPerDim;
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
                index = indexCalculator(i, j, k);
                // index = i * pow(this->numOfCellsPerDim, 2) + j * this->numOfCellsPerDim + k;
                this->kSquare[index] = (k * k + j * j + i * i) / this->wSquare;
            }
        }
    }
    this->fNorm = 1.0 / (8 * this->numOfCells);

    this->densityBuffer = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * this->numOfCells);
    this->potentialBuffer = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * this->numOfCells);
    this->frequencyBuffer = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * this->numOfCells);
    this->potentialRealPart = std::vector<double>(this->numOfCells);
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
    // double intePart = 0.0;
    double quotient = 0.0;
    int index = 0;
    for (int i = 0; i < 3; ++i)
    {
        // std::modf(position[i] / this->relCellWidth, &intePart); // get the integer part from the division to get the index of each coordinates of the cell
        quotient = std::floor(position[i] / this->relCellWidth);
        index += quotient * pow(this->numOfCellsPerDim, 2 - i); // get the index of the cell in the 1-d array
        // std::cout << quotient << " ";
    }
    // std::cout << "quotient" << std::endl;
    return static_cast<int>(index);
}

int Simulation::wrapHelper(int i)
{
    // if (i > this->numOfCellsPerDim)
    if (i == this->numOfCellsPerDim)

    {
        // std::cout << "error!!!!!!!!!!!!!!!" << std::endl;
        return 0;
    }
    else if (i == -1)
    {
        // std::cout << "error!!!!!!!!!!!!!!!" << std::endl;
        // return this->numOfCellsPerDim;
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

void Simulation::densityCalculator()
{
    // double normalizedPos = 0.0;
    // double intePart = 0.0;
    int index = 0;

    // memset(this->densityBuffer, 0.0, sizeof(fftw_complex) * this->numOfCells); // initialize the buffer with all entry to be 0.0

    for (int i = 0; i < this->numOfCells; ++i)
    {
        this->densityBuffer[i][0] = 0.0; // set the real part 0.0
        this->densityBuffer[i][1] = 0.0; // set the imagine part 0.0
    }

    for (auto iter = this->particlesSimu.particleInfo.begin(); iter < this->particlesSimu.particleInfo.end(); iter++)
    {
        // index = 0;
        // for (int i = 0; i < 3; ++i)
        // {
        //     std::modf(iter->position[i] / this->relCellWidth, &intePart); // get the integer part from the division to get the index of each coordinates of the cell
        //     index += intePart * pow(this->numOfCellsPerDim, 2 - i);       // get the index of the cell in the 1-d array
        // }
        index = cellIdentifier(iter->position);
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

    for (int i = 0; i < this->numOfCells; i++)
    {
        this->potentialRealPart[i] = this->potentialBuffer[i][0];
    }
}

void Simulation::accelerationCalculator()
{
    int index = 0;
    for (int i = 0; i < this->numOfCellsPerDim; i++)
    {
        for (int j = 0; j < this->numOfCellsPerDim; j++)
        {
            for (int k = 0; k < this->numOfCellsPerDim; k++)
            {
                index = indexCalculator(i, j, k);

                this->acceleration[index][0] = (this->potentialRealPart[indexCalculator(wrapHelper(i - 1), j, k)] - this->potentialRealPart[indexCalculator(wrapHelper(i + 1), j, k)]) / (2.0 * this->cellWidth);
                this->acceleration[index][1] = (this->potentialRealPart[indexCalculator(i, wrapHelper(j - 1), k)] - this->potentialRealPart[indexCalculator(i, wrapHelper(j + 1), k)]) / (2.0 * this->cellWidth);
                this->acceleration[index][2] = (this->potentialRealPart[indexCalculator(i, j, wrapHelper(k - 1))] - this->potentialRealPart[indexCalculator(i, j, wrapHelper(k + 1))]) / (2.0 * this->cellWidth);
            }
        }
    }
}

void Simulation::particlesUpdater()
{
    int index = 0;
    for (auto iter = this->particlesSimu.particleInfo.begin(); iter < this->particlesSimu.particleInfo.end(); iter++)
    {
        index = cellIdentifier(iter->position);
        // std::cout << iter->position[0] << " " << iter->position[1] << " " << iter->position[2] << " " << index << std::endl;
        iter->updater(this->acceleration[index]);
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
    int index = 0;
    for (int i = 0; i < this->numOfCellsPerDim; i++)
    {
        for (int j = 0; j < this->numOfCellsPerDim; j++)
        {
            for (int k = 0; k < this->numOfCellsPerDim; k++)
            {
                index = indexCalculator(i, j, k);
                // index = i * pow(this->numOfCellsPerDim, 2) + j * this->numOfCellsPerDim + k;
                this->kSquare[index] = (k * k + j * j + i * i) / this->wSquare;
            }
        }
    }

    for (auto iter = this->particlesSimu.particleInfo.begin(); iter < this->particlesSimu.particleInfo.end(); iter++)
    {
        iter->velocityRescaler(this->expanFac);
    }
}

void Simulation::run(std::optional<std::string> folderPath)
{
    int iteration = -1;
    bool flag = false;
    if (folderPath.has_value())
    {
        flag = true;
        if (!std::filesystem::exists(*folderPath))
        {
            std::filesystem::create_directories(*folderPath);
        }
    }

    for (double i = 0.0; i < this->timeMax; i += this->timeStep)
    {
        densityCalculator();
        potentialCalculator();
        accelerationCalculator();
        particlesUpdater();
        boxExpander();
        iteration++;
        if (iteration % 10 == 0)
        {
            if (flag == true)
            {
                SaveToFile(this->densityBuffer, this->numOfCellsPerDim, *folderPath);
            }
        }
    }
}