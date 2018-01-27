#pragma once

#include "EntityRenderable.hpp"

class Entity
{
public:
  enum class Type
  {
    BOMBER,
    MELEE,
    FARMER,
    SHOOTER,
  };

  static constexpr double RANGE = 5.0;
  Type type;
  EntityRenderable renderable;

  Entity(Type type);
  ~Entity() = default;

  void shooter();
};
