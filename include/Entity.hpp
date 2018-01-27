#pragma once

#include "Fixture.hpp"

class Entity
{
public:
  static constexpr double PROPAG_RANGE = 5.0;
  Fixture fixture;
  bool dead;

  Entity();
  ~Entity() = default;
};
