#pragma once

#include <map>
#include <set>
#include "Entity.hpp"
#include "TextureHandler.hpp"

class EntityManager;
class Logic;

class NanoBot : public Entity
{
private:
  unsigned int cooldown;

public:
  using CollisionContainer = std::map<Entity *, std::set<Entity *>>;

  enum Type : std::size_t
  {
    WORKER,
    BRUTE,
    SHOOTER,
    BOMBER,
  };

  Type type;

  NanoBot(Type type);
  void update();
  void ia(EntityManager& em);

  void workerAction(CollisionContainer &, Logic &);
  void bruteAction(CollisionContainer &, Logic &);
  void shooterAction(CollisionContainer &, Logic &);
  void bomberAction(CollisionContainer &, Logic &);

  TextureHandler::TextureList getTexture() const noexcept;
};
