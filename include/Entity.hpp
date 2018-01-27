#pragma once

#include "Fixture.hpp"

class Entity
{
public:
  static constexpr double PROPAG_RANGE = 0.05;
  Fixture fixture;
  bool dead;

  constexpr Entity() noexcept
    : dead(false)
  {
  }

  ~Entity() = default;
};
