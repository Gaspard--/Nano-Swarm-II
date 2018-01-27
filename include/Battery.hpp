#pragma once

#include "Entity.hpp"
#include "TextureHandler.hpp"

class Battery : public Entity
{
private:
  int power;

public:
  Battery(int power=100);
  ~Battery() = default;

  auto getTexture() const noexcept
  {
    return TextureHandler::TextureList::BATTERY;
  }
};
