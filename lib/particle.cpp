#include "particle.hpp"

particle::particle(std::vector<double> positionNew)
{
    this->positionSetter(positionNew);
}

double particle::massGetter()
{
    return particle::mass;
}

void particle::massSetter(double val)
{
    particle::mass = val;
}

void particle::dtSetter(double dt)
{
    particle::dt = dt;
}

void particle::positionSetter(std::vector<double> positionNew)
{
    double intePart;
    for (int i = 0; i < 3; ++i)
    {
        if (positionNew[i] < 0.0)
        {
            // std::cout << "smaller than 0.0 ......................................" << std::endl;
            position[i] = std::modf(positionNew[i], &intePart) + 1.0;
            // if (position[i] == 1.0)
            // {
            //     position[i] = 0.0;
            //     std::cout << "................................................................................." << std::endl;
            // }
        }
        else if (positionNew[i] >= 1.0)
        {
            // std::cout << "equal or larger than 1.0 ......................................" << std::endl;
            position[i] = std::modf(positionNew[i], &intePart);
        }
        else
        {
            position[i] = positionNew[i];
        }
    }
    // std::cout << "input: " << positionNew[0] << " " << positionNew[1] << " " << positionNew[2] << std::endl;
    // std::cout << "posi: " << position[0] << " " << position[1] << " " << position[2] << std::endl;
}

void particle::updater(std::vector<double> acceleration)
{
    // std::vector<double> positionNew;
    for (int i = 0; i < 3; i++)
    {
        this->velocity[i] += acceleration[i] * this->dt;
        this->position[i] += this->velocity[i] * this->dt;
        // positionNew.push_back(this->position[i] + this->velocity[i] * this->dt);
    }
    positionSetter(this->position);
}

void particle::velocityRescaler(double expanFac)
{
    for (int i = 0; i < 3; i++)
    {
        this->velocity[i] /= expanFac;
    }
}
