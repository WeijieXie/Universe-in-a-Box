#include "data.hpp"

const particles test_data::initParticlesEmpty = particles();
const particles test_data::initParticlesRandom = particles(100, 23093556);
const particles test_data::initParticlesOneInMid = particles({{0.5, 0.5, 0.5}});