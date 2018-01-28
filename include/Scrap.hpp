#pragma once

#include <vector>
#include "claws/Vect.hpp"
#include "NanoBot.hpp"
#include "TeamEntities.hpp"

class Scrap
{
  Fixture fixture;
  NanoBot::Type type;

public:
  Scrap(claws::Vect<2u, double>, claws::Vect<2u, double>, NanoBot::Type);
  ~Scrap();
  void update(auto begin, auto end, std::vector<TeamEntity<NanoBot, true>>);
};
