#include "Scrap.hpp"

Scrap::Scrap(Claws::Vect<2u, double> pos, Claws::Vect<2u, double> speed, NanoBot::Type type)
  : fixture(pos, speed, {0.0f, 0.0f})
{
}

void Scrap::update(auto begin, auto end, std::Vector<TeamEntity<NanoBot, true>>)
{
  std::vector<int> dists;
  int i(0);
  int best(0);

  for (auto it = begin; it != end; it++)
    {
      dists.push_back((fixture.pos - it.fixture.pos).length2());
    }

  int min(dists[0]);
  for (auto it : dists)
    {
      if (it < min)
	{
	  min = it;
	  best = i;
	}
      i += 1;
    }
  target = (begin + best).fixture.pos;
}
