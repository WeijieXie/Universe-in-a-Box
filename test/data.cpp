#include "data.hpp"

const double test_data::timeMax = 1.0;
const double test_data::timeStep = 0.1;
const double test_data::mass = 0.01;
const double test_data::width = 100;
const double test_data::expanFac = 1;

const int test_data::numOfCellsPerDim = 2;
const double test_data::densityContributionPerParticle = 0.01 / (125000.0);

const particles test_data::initParticlesEmpty = particles();
const particles test_data::initParticlesRandom = particles(100, 23093556);
const particles test_data::initParticlesOneInMid = particles({{0.5, 0.5, 0.5}});     // 111
const particles test_data::initParticlesMultiple = particles({{0.0, 0.0, 0.0},       // 000
                                                              {0.001, 0.001, 0.001}, // 000
                                                              {0.001, 0.001, 0.001}, // 000
                                                              {0.001, 0.001, 0.001}, // 000
                                                              {0.02, 0.2, 0.54},     // 001
                                                              {0.02, 0.4, 0.87},     // 001
                                                              {0.32, 0.0, 0.888},    // 001
                                                              {0.02, 0.0, 0.678},    // 001
                                                              {0.02, 0.7, 0.2},      // 010
                                                              {0.02, 0.6, 0.7},      // 011
                                                              {0.02, 0.55, 0.7},     // 011
                                                              {0.999, 0.999, 0.999}, // 111
                                                              {0.999, 0.999, 0.999}, // 111
                                                              {0.999, 0.999, 0.9}}); // 111

const fftw_complex test_data::densityBufferForSingleParticle[] = {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {1 * densityContributionPerParticle, 0.0}};
const fftw_complex test_data::densityBufferForMultipleParticles[] = {{4 * densityContributionPerParticle, 0.0}, {4 * densityContributionPerParticle, 0.0}, {1 * densityContributionPerParticle, 0.0}, {2 * densityContributionPerParticle, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {3 * densityContributionPerParticle, 0.0}};

// for gradient tests
const std::vector<std::vector<double>> test_data::zeroGradient = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
fftw_complex test_data::potential_27_data[27] = {{1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {1.0, 0.0}};
fftw_complex *test_data::potential_27 = test_data::potential_27_data;
const std::vector<std::vector<double>> test_data::gradient_27 = {{0.0, 0.0, 0.0}, {0.0, 0.0, 1.0 / (test_data::width / 3 * 2)}, {0.0, 0.0, -1.0 / (test_data::width / 3 * 2)}, {0.0, 1.0 / (test_data::width / 3 * 2), 0.0}, {-1.0 / (test_data::width / 3 * 2), 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, -1.0 / (test_data::width / 3 * 2), 0.0}, {0.0, 0.0, 0.0}, {1.0 / (test_data::width / 3 * 2), 0.0, 0.0}, {1.0 / (test_data::width / 3 * 2), 0.0, 0.0}, {0.0, -1.0 / (test_data::width / 3 * 2), 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, -1.0 / (test_data::width / 3 * 2)}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0 / (test_data::width / 3 * 2)}, {0.0, 0.0, 0.0}, {0.0, 1.0 / (test_data::width / 3 * 2), 0.0}, {-1.0 / (test_data::width / 3 * 2), 0.0, 0.0}, {-1.0 / (test_data::width / 3 * 2), 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 1.0 / (test_data::width / 3 * 2), 0.0}, {0.0, 0.0, 0.0}, {1.0 / (test_data::width / 3 * 2), 0.0, 0.0}, {0.0, -1.0 / (test_data::width / 3 * 2), 0.0}, {0.0, 0.0, 1.0 / (test_data::width / 3 * 2)}, {0.0, 0.0, -1.0 / (test_data::width / 3 * 2)}, {0.0, 0.0, 0.0}};

// for updater tests
const particles test_data::initParticlesAB = particles({{0.01, 0.01, 0.01}, {0.6, 0.6, 0.6}});
const std::vector<std::vector<double>> test_data::acceleration = {{-10, -10, -10}, {-2, -1, -3}, {3, 9, 7}, {-10, -10, -10}, {-2, -1, -3}, {3, 9, 7}, {-10, -10, -10}, {-2, -1, -3}};
const std::vector<double> test_data::partilceAPos = {0.79, 0.8, 0.78};
const std::vector<double> test_data::partilceASpeed = {-1.2, -1.1, -1.3};
const std::vector<double> test_data::partilceBPos = {0.54, 0.57, 0.51};
const std::vector<double> test_data::partilceBSpeed = {-0.4, -0.2, -0.6};