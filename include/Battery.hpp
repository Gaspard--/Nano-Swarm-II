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

  auto getTexture() const noexcept
  {
    return TextureHandler::TextureList::BATTERY;
  }
};
