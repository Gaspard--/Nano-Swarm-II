#pragma once

#include "claws/Vect.hpp"

struct Laser
{
  claws::Vect<2u, double> start;
  claws::Vect<2u, double> end;
  double power;

  Laser(claws::Vect<2u, double> start, claws::Vect<2u, double> end, double power)
    : start(start), end(end), power(power)
  {
  }
};
