#pragma once

#include <vector>
#include "Entity.hpp"
#include "Battery.hpp"

typedef struct	TeamEntities
{
  std::vector<Entity> mobs;
  std::vector<Battery> batteries;
}		Team;

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
