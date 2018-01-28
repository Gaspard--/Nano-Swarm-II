#pragma once

#include <map>
#include <set>
#include "Entity.hpp"
#include "TextureHandler.hpp"

class EntityManager;
class Logic;
class Battery;

class NanoBot : public Entity
{
private:
  unsigned int cooldown;

public:
  bool hasPlayed;
  
  enum Type : std::size_t
  {
    WORKER,
    BRUTE,
    SHOOTER,
    BOMBER,
  };

  Type type;

  constexpr NanoBot(Type type)
  : cooldown(0u),
    hasPlayed(true),
    type(type)
  {
  }

  void update();
  
  template<class CollisionContainer, class CollisionContainer2, class Access>
  inline void ia(CollisionContainer &nearBots, CollisionContainer2 &nearBatteries, Battery &source, Access &access, Logic &logic);

  template<class CollisionContainer, class Access>
  void workerAction(CollisionContainer &, Battery &, Access &, Logic &);

  template<class CollisionContainer, class Access>
  void bruteAction(CollisionContainer &, Battery &, Access &, Logic &);

  template<class CollisionContainer, class Access>
  void shooterAction(CollisionContainer &, Battery &, Access &, Logic &);

  template<class CollisionContainer, class Access>
  void bomberAction(CollisionContainer &, Battery &, Access &, Logic &);

  TextureHandler::TextureList getTexture(bool team) const noexcept;
};
