#pragma once

#include "claws/Vect.hpp"
#include "NanoBot.hpp"

class Scrap
{
  Fixture fixture;
  NanoBot::Type type;

public:
  Scrap(Claws::Vect<2u, double>, Claws::Vect<2u, double>, NanoBot::Type);
  ~Scrap();
  void update(auto begin, auto end, std::Vector<TeamEntity<NanoBot, true>>);
};
