#pragma once

#include "Fixture.hpp"

class Entity
{
public:
  static constexpr double PROPAG_RANGE = 0.05;
  Fixture fixture;
  bool dead;
  bool selected;

  constexpr Entity() noexcept
  : dead(false), selected(false)
  {
  }

  void setSelection(bool selected);

  ~Entity() = default;
};
