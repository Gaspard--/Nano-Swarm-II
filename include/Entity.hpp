#pragma once

#include "EntityRenderable.hpp"

class Entity
{
public:
  enum class Type
  {
    BOMBER,
    MELEE,
    FARMER
  };

  static constexpr double RANGE = 5.0;
  EntityRenderable renderable;

  Entity();
  ~Entity() = default;
};
