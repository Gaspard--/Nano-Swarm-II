#ifndef ENTITYMANAGER_HPP_
# define ENTITYMANAGER_HPP_

#include <vector>
#include "Entity.hpp"
#include "Battery.hpp"

class	EntityManager
{
public:
  EntityManager();
  ~EntityManager() = default;
  std::vector<Entity> allies;
  std::vector<Battery> batteriesA;

  std::vector<Entity> ennemies;
  std::vector<Battery> batteriesE;

  std::vector<Battery> pylones;

private:
};

#endif //!ENTITYMANAGER_HPP_
