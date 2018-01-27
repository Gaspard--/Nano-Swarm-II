#pragma once

#include "TextureHandler.hpp"

struct  TeamEntities
{
  std::vector<Entity> units;
  std::vector<Battery> batteries;

  TeamEntities() = default;
  ~TeamEntities() = default;

  template<class Lambda>
  void iterOnTeam(Lambda lambda, TextureHandler const &textureHandler)
  {
    for (auto &&it : units)
      {
	lambda(textureHandler.getTexture(TextureHandler::TextureList::UNIT));
      }
    for (auto &&it : batteries)
      {
	lambda(textureHandler.getTexture(TextureHandler::TextureList::BATTERY));
      }
  }
};
