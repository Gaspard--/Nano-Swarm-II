#pragma once

#include "Entity.hpp"
#include "TextureHandler.hpp"

class EntityManager;

class Battery : public Entity
{
private:
  unsigned int power;
  constexpr static unsigned int maxPower = 100;

public:
  Battery(unsigned int power = maxPower);
  ~Battery() = default;

  void reload(Battery &source);
  void update();
  void ia(EntityManager& em);
  void setPower(unsigned int power);
  int getPower() const;

  auto getTexture(bool team) const noexcept
  {
    return team ? TextureHandler::TextureList::BATTERY
      : TextureHandler::TextureList::BATTERY_ENEMY;
  }
};
