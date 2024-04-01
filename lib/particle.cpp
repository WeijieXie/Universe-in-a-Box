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
            position[i] = 1.0 + std::fmod(positionNew[i], 1.0); // if the position is negative, it is wrapped around the box

            if (position[i] == 1.0)
            {
                position[i] = 0.0; // to avoid floating point errors
            }
        }
        else if (positionNew[i] >= 1.0)
        {
            position[i] = std::fmod(positionNew[i], 1.0); // if the position is greater than 1, it is wrapped around the box
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
        this->velocity[i] += acceleration[i] * this->dt;                         // updating the velocity
        positionNew.push_back(this->position[i] + this->velocity[i] * this->dt); // updating the position
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
