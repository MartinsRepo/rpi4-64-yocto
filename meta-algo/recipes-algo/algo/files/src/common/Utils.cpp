#include "common/Utils.hpp"

#include <iostream>
#include <fstream>
#include <math.h>

void  Utils::ftanh(float &value) { value = tanh(value); }

void  Utils::fsigmoid(float &value) 
{
  value = 1 / (1 + exp(clip(-value, -20.0, 20.0)));
}

float  Utils::clip(float n, float lower, float upper)
{
  return std::max(lower, std::min(n, upper));
}