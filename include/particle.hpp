#pragma once

#include <vector>
#include <cmath>

class particle
{
private:
    static double mass;

public:
    std::vector<double> position = {0.0, 0.0, 0.0};
    std::vector<double> velocity = {0.0, 0.0, 0.0};
    // std::vector<double> acceleration = {0.0, 0.0, 0.0};

    particle()=default;
    particle(std::vector<double>);

    static void massSetter(double mass);
    void positionSetter(std::vector<double> positionNew);
};

double particle::mass = 1.0;
