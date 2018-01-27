#pragma once

#include "claws/Vect.hpp"

struct Fixture
{
  static constexpr double getRadius() noexcept
  {
    return 0.05;
  }

  claws::Vect<2u, double> pos;
  claws::Vect<2u, double> speed;
  claws::Vect<2u, double> target;

  constexpr void update() noexcept
  {
    pos += speed;
  }
};
