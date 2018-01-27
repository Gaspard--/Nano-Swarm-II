#pragma once

#include "TextureHandler.hpp"
#include "NanoBot.hpp"

struct  TeamEntities
{
  std::vector<NanoBot> units;
  std::vector<Battery> batteries;

  TeamEntities() = default;
  ~TeamEntities() = default;

  template<class Lambda>
  void iterOnTeam(Lambda lambda, TextureHandler const &textureHandler)
  {
    for (auto &&it : units)
      {
	lambda(textureHandler.getTexture(TextureHandler::TextureList::UNIT), it.fixture);
      }
    for (auto &&it : batteries)
      {
	lambda(textureHandler.getTexture(TextureHandler::TextureList::BATTERY), it.fixture);
      }
  }
};
