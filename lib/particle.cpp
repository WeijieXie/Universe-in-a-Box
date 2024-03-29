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
    for (int i = 0; i < 3; ++i)
    {
        if (positionNew[i] < 0.0)
        {
            position[i] = 1.0 + std::fmod(positionNew[i], 1.0);
            if (position[i] == 1.0)
            {
                position[i] = 0.0;
            }
        }
        else if (positionNew[i] >= 1.0)
        {
            position[i] = std::fmod(positionNew[i], 1.0);
        }
        else
        {
            position[i] = positionNew[i];
        }
    }
}

void particle::updater(std::vector<double> acceleration)
{
    std::vector<double> positionNew;
    for (int i = 0; i < 3; i++)
    {
        this->velocity[i] += acceleration[i] * this->dt;
        positionNew.push_back(this->position[i] + this->velocity[i] * this->dt);
    }
    positionSetter(positionNew);
}

void particle::velocityRescaler(double expanFac)
{
    for (int i = 0; i < 3; i++)
    {
        this->velocity[i] /= expanFac;
    }
}
