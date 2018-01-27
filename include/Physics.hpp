#pragma once

#include <algorithm>
#include <numeric>
#include <tuple>
#include "claws/Vect.hpp"

template<class Solver>
struct Physics
{
  Solver &solver;

private:
  using expander = int[];

  template<class First>
  auto getMinMaxImpl(std::tuple<First> begin, std::tuple<First> end)
  {
    auto copy(std::get<First>(begin));
    auto start(*copy);

    return std::accumulate(++copy, std::get<First>(end), start, [](auto minmax, auto const &elem) {
	for (std::size_t i(0ul); i != 2ul; ++i)
	  {
	    minmax.first[i] = std::min(minmax.first[i], elem.pos[i]);
	    minmax.second[i] = std::max(minmax.second[i], elem.pos[i]);
	  }
	return minmax;
      });
  }

  template<class First, class... It>
  auto getMinMaxImpl(std::tuple<First, It...> begin, std::tuple<First, It...> end)
  {
    return std::accumulate(std::get<First>(begin), std::get<First>(end), getMinMaxImpl({std::get<It>(begin)...}, {std::get<It>(end)...}),
			   [](auto minmax, auto const &elem)
			   {
			     for (std::size_t i(0ul); i != 2ul; ++i)
			       {
				 minmax.first[i] = std::min(minmax.first[i], elem.pos[i]);
				 minmax.second[i] = std::max(minmax.second[i], elem.pos[i]);
			       }
			     return minmax;
			   });
  }

  template<class... It>
  void classicSolve(std::tuple<It...> begin, std::tuple<It...> end)
  {
    auto checkOthers([this](auto begin, auto end, std::tuple<It...> otherBegins, std::tuple<It...> otherEnds)
		     {
		       auto check([this](auto begin1, auto end1, auto begin2, auto end2)
				  {
				    for (auto it1(begin1); it1 < end1; ++it1)
				      for (auto it2(begin2); it2 < end2; ++it2)
					solver(*it1, *it2);
				  });
		       (void)expander{check(begin, end, std::get<It>(otherBegins), std::get<It>(otherEnds))...};
		     });
    (void)expander{checkOthers(std::get<It>(begin), std::get<It>(end), {std::get<It>(begin)...},  {std::get<It>(end)...})...};
  }

  template<class... It>
  void checkCollisionImpl(std::tuple<It...> begin, std::tuple<It...> end)
  {
    claws::Vect<2u, double> min;
    claws::Vect<2u, double> max;
    std::tie(min, max) = getMinMaxImpl(begin, end);
    claws::Vect<2u, double> mid((min + max) * 0.5);

    if (claws::Vect<2u, std::size_t>{(std::get<It>(begin) - std::get<It>(end))...}.sum() < 20)
      return classicSolve(begin, end);
    for (std::size_t i(0ul); i != 2ul; ++i)
      {
	auto isBelow([mid, i](auto const &a){
	      return a->pos[i] + a->radius > a->mid[i];
	  });
	auto isAbove([mid, i](auto const &a){
	      return a->pos[i] - a->radius < a->mid[i];
	  });
	for (auto pred : {isBelow, isAbove})
	  checkCollisionImpl(begin, {std::partition(std::get<It>(begin), std::get<It>(end), pred)...});
      }
  }

public:
  template<class... T>
  void prepareCollisionCheck(std::tuple<T...> begin, std::tuple<T...> end)
  {
    std::tuple<std::vector<decltype(&*std::get<T>(begin))>...> ptrStorage;

    (void)expander{(std::get<std::vector<decltype(&*std::get<T>(begin))>>(ptrStorage).resize(std::get<T>(end) - std::get<T>(begin)), 0)...};
    (void)expander{(std::transform(std::get<T>(begin), std::get<T>(end), std::get<std::vector<decltype(&*std::get<T>(begin))>>(ptrStorage).begin(), [](auto &a){return &a;}), 0)...};
    checkCollisionImpl({std::get<std::vector<decltype(&*std::get<T>(begin))>>(ptrStorage).begin()...},
		       {std::get<std::vector<decltype(&*std::get<T>(begin))>>(ptrStorage).end()...});
  }

  template<class It1, class It2>
  void checkCollision(It1 begin1, It1 end1, It2 begin2, It2 end2)
  {
    checkCollisionImpl({begin1, begin2}, {end1, end2});
  }
};
