#pragma once

#include <algorithm>
#include <numeric>
#include <tuple>
#include "claws/Vect.hpp"
#include <iostream>

template<class Solver>
struct Physics
{
  Solver &solver;

private:
  using expander = int[];

  auto getMinMaxImpl(std::tuple<>, std::tuple<>)
  {
    return std::pair<claws::Vect<2u, double>, claws::Vect<2u, double>>({std::numeric_limits<double>::max(), std::numeric_limits<double>::max()},
								       {std::numeric_limits<double>::min(), std::numeric_limits<double>::min()});
  }

  template<class First, class... It>
  auto getMinMaxImpl(std::tuple<First, It...> begin, std::tuple<First, It...> end)
  {
    return std::accumulate(std::get<First>(begin), std::get<First>(end), getMinMaxImpl(std::tuple<It...>{std::get<It>(begin)...}, std::tuple<It...>{std::get<It>(end)...}),
			   [](auto minmax, auto const &elem)
			   {
			     for (std::size_t i(0ul); i != 2ul; ++i)
			       {
				 minmax.first[i] = std::min(minmax.first[i], elem->fixture.pos[i]);
				 minmax.second[i] = std::max(minmax.second[i], elem->fixture.pos[i]);
			       }
			     return minmax;
			   });
  }

  template<class... It>
  void classicSolve(std::tuple<It...> const begin, std::tuple<It...> const end)
  {
    auto checkOthers([this](auto begin, auto end, std::tuple<It...> otherBegins, std::tuple<It...> otherEnds)
		     {
		       auto check([this](auto begin1, auto end1, auto begin2, auto end2)
				  {
				    for (auto it1(begin1); it1 < end1; ++it1)
				      for (auto it2(begin2); it2 < end2; ++it2)
					if (((*it1)->fixture.pos - (*it2)->fixture.pos).length2() <
					    (*it1)->fixture.getRadius() * (*it1)->fixture.getRadius() +
					    (*it2)->fixture.getRadius() * (*it2)->fixture.getRadius())
					  solver(**it1, **it2);
				  });
		       (void)expander{(check(begin, end, std::get<It>(otherBegins), std::get<It>(otherEnds)), 0)...};
		     });
    (void)expander{(checkOthers(std::get<It>(begin), std::get<It>(end), {std::get<It>(begin)...},  {std::get<It>(end)...}), 0)...};
  }

  template<class... It>
  void checkCollisionImpl(std::tuple<It...> const begin, std::tuple<It...> const end, std::size_t level)
  {
    claws::Vect<2u, double> min;
    claws::Vect<2u, double> max;
    std::tie(min, max) = getMinMaxImpl(begin, end);
    claws::Vect<2u, double> mid((min + max) * 0.5);

    if (claws::Vect<sizeof...(It), std::size_t>{static_cast<std::size_t>((std::get<It>(end) - std::get<It>(begin)))...}.sum() < 20ul || level >= 20)
      return classicSolve(begin, end);
    for (std::size_t i(0ul); i != 2ul; ++i)
      {
	auto isBelow([mid, i](auto const &a){
	    return a->fixture.pos[i] + a->fixture.getRadius() > mid[i];
	  });
	auto isAbove([mid, i](auto const &a){
	    return a->fixture.pos[i] - a->fixture.getRadius() < mid[i];
	  });
	checkCollisionImpl(begin, {std::partition(std::get<It>(begin), std::get<It>(end), isBelow)...}, level + 1);
	checkCollisionImpl(begin, {std::partition(std::get<It>(begin), std::get<It>(end), isAbove)...}, level + 1);
      }
  }

  template<class... T>
  void prepareCollisionCheck(std::tuple<T...> begin, std::tuple<T...> end)
  {
    std::tuple<std::vector<decltype(&*std::get<T>(begin))>...> ptrStorage;

    (void)expander{(std::get<std::vector<decltype(&*std::get<T>(begin))>>(ptrStorage).resize(std::get<T>(end) - std::get<T>(begin)), 0)...};
    (void)expander{(std::transform(std::get<T>(begin), std::get<T>(end), std::get<std::vector<decltype(&*std::get<T>(begin))>>(ptrStorage).begin(), [](auto &a){return &a;}), 0)...};
    checkCollisionImpl(std::tuple<typename std::vector<decltype(&*std::get<T>(begin))>::iterator...>{std::get<std::vector<decltype(&*std::get<T>(begin))>>(ptrStorage).begin()...},
		       std::tuple<typename std::vector<decltype(&*std::get<T>(begin))>::iterator...>{std::get<std::vector<decltype(&*std::get<T>(begin))>>(ptrStorage).end()...},
		       0);
  }

public:
  template<class... Container>
  void checkCollision(Container &... container)
  {
    prepareCollisionCheck(std::tuple<typename Container::iterator...>{container.begin()...}, std::tuple<typename Container::iterator...>{container.end()...});
  }
};

template<class Solver>
auto makePhysics(Solver &solver)
{
  return Physics<Solver>{solver};
}
