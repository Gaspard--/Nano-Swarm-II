#pragma once

#include <vector>
#include "Entity.hpp"
#include "Battery.hpp"
#include "Physics.hpp"
#include "TeamEntities.hpp"

class	EntityManager
{
public:
  EntityManager();
  ~EntityManager() = default;

  TeamEntities allies;
  TeamEntities ennemies;
  std::vector<Battery> pylones;

private:
};
