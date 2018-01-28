#include "Scrap.hpp"

Scrap::Scrap(claws::Vect<2u, double> pos, claws::Vect<2u, double> speed, NanoBot::Type type)
  : fixture{pos, speed, claws::Vect<2u, double>{0.0f, 0.0f}}, type(type)
{
}

void Scrap::update(auto begin, auto end, std::vector<TeamEntity<NanoBot, true>>)
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
  fixture.target = (begin + best).fixture.pos;
}
