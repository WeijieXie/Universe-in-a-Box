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

void particle::positionSetter(std::vector<double> positionNew)
{
    double intePart;
    for (int i = 0; i < 3; ++i)
    {
        if (positionNew[i] < 0)
        {
            position[i] = std::modf(positionNew[i], &intePart) + 1;
        }
        else if (positionNew[i] >= 1)
        {
            position[i] = std::modf(positionNew[i], &intePart);
        }
        else
        {
            position[i] = positionNew[i];
        }
    }
}
