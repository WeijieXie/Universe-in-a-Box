#include "data.hpp"

const double test_data::timeMax = 1.0;
const double test_data::timeStep = 0.1;
const double test_data::mass = 0.01;
const double test_data::width = 100;
const double test_data::expanFac = 1;

const int test_data::numOfCellsPerDim = 2;
const double test_data::densityContributionPerParticle = 0.01/(125000.0);

const particles test_data::initParticlesEmpty = particles();
const particles test_data::initParticlesRandom = particles(100, 23093556);
const particles test_data::initParticlesOneInMid = particles({{0.5, 0.5, 0.5}}); // 111
const particles test_data::initParticlesMultiple = particles({{0.0,0.0,0.0}, // 000
                                                              {0.001,0.001,0.001}, // 000
                                                              {0.001,0.001,0.001}, // 000
                                                              {0.001,0.001,0.001}, // 000
                                                              {0.02, 0.2, 0.54}, // 001
                                                              {0.02, 0.4, 0.87}, // 001
                                                              {0.32, 0.0, 0.888}, // 001
                                                              {0.02, 0.0, 0.678}, // 001
                                                              {0.02, 0.7, 0.2}, // 010
                                                              {0.02, 0.6, 0.7}, // 011
                                                              {0.02, 0.55, 0.7}, // 011
                                                              {0.999,0.999,0.999}, // 111
                                                              {0.999,0.999,0.999}, // 111
                                                              {0.999,0.999,0.9}}); // 111

const fftw_complex test_data::densityBufferForSingleParticle[] = {{0.0,0.0},{0.0,0.0},
                                                                  {0.0,0.0},{0.0,0.0},
                                                                  {0.0,0.0},{0.0,0.0},
                                                                  {0.0,0.0},{1*densityContributionPerParticle,0.0}};
const fftw_complex test_data::densityBufferForMultipleParticles[] = {{4*densityContributionPerParticle,0.0},{0.0,0.0},
                                                                     {1*densityContributionPerParticle,0.0},{0.0,0.0},
                                                                     {4*densityContributionPerParticle,0.0},{0.0,0.0},
                                                                     {2*densityContributionPerParticle,0.0},{3*densityContributionPerParticle,0.0}};