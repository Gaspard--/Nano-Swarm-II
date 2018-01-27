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
  void iterOnTeam(Lambda lambda)
  {
    for (auto &&it : units)
      {
	lambda(it);
      }
    for (auto &&it : batteries)
      {
	lambda(it);
      }
  }
};
