#pragma once

#include "Entity.hpp"
#include "TextureHandler.hpp"

class EntityManager;

class Battery : public Entity
{
private:
  int power;
  constexpr static int maxPower = 100;

public:
  Battery(int power = 100);
  ~Battery() = default;

  void update();
  void ia(EntityManager& em);
  void setPower(int power);
  int getPower() const;

  auto getTexture(bool team) const noexcept
  {
    return team ? TextureHandler::TextureList::BATTERY
      : TextureHandler::TextureList::BATTERY_ENEMY;
  }
};
