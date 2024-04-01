#pragma once

#include <vector>
#include <cmath>

// Class representing a particle
class particle
{
private:
    static inline double mass = 1.0; // mass of the particle
    static inline double dt = 0.1;   // time step used for updating the position and velocity

public:
    std::vector<double> position = {0.0, 0.0, 0.0};
    std::vector<double> velocity = {0.0, 0.0, 0.0};

    particle() = default;
    particle(std::vector<double>);

    static double massGetter();
    static void massSetter(double mass);
    static void dtSetter(double dt);
    void positionSetter(std::vector<double> positionNew); // sets the position of the particle
    void updater(std::vector<double> acceleration);
    void velocityRescaler(double expanFac);
};