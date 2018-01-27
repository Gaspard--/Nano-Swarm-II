#pragma once

#include "TextureHandler.hpp"
#include "NanoBot.hpp"

template<class Base, bool team>
struct TeamEntity : public Base
{
  constexpr static auto getTeam() noexcept
  {
    return team;
  }

  using Base::Base;
};

template<bool team>
struct  TeamEntities
{
  std::vector<TeamEntity<NanoBot, team>> units;
  std::vector<TeamEntity<Battery, team>> batteries;

  TeamEntities() = default;
  ~TeamEntities() = default;

  template<class Lambda>
  void iterOnTeam(Lambda lambda) const
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

  template<class Lambda>
  void iterOnTeam(Lambda lambda)
  {
    for (auto &it : units)
      {
	lambda(it);
      }
    for (auto &it : batteries)
      {
	lambda(it);
      }
  }
};
