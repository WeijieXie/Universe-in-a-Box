#pragma once

#include <vector>
#include <cmath>

class particle
{
private:
    static inline double mass = 1.0;
    static inline double dt = 0.1;

public:
    std::vector<double> position = {0.0, 0.0, 0.0};
    std::vector<double> velocity = {0.0, 0.0, 0.0};
    // std::vector<double> acceleration = {0.0, 0.0, 0.0};

    particle() = default;
    particle(std::vector<double>);

    static double massGetter();
    static void massSetter(double mass);
    static void dtSetter(double dt);
    void positionSetter(std::vector<double> positionNew);
    void updater(std::vector<double> acceleration);
};

// double particle::mass = 1.0;
