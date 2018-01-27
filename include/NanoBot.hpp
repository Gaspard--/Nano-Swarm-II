#pragma once

#include <map>
#include <set>
#include "Entity.hpp"

class Logic;

class NanoBot : public Entity
{
private:
  unsigned int cooldown;

public:
  using CollisionContainer = std::map<Entity *, std::set<Entity *>>;

  enum class Type
  {
    WORKER,
    BRUTE,
    SHOOTER,
    BOMBER,
  };

  Type type;

  NanoBot(Type type);

  void workerAction(CollisionContainer &, Logic &);
  void bruteAction(CollisionContainer &, Logic &);
  void shooterAction(CollisionContainer &, Logic &);
  void bomberAction(CollisionContainer &, Logic &);
};
