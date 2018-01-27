#pragma once

#include <algorithm>
#include <numeric>
#include "claws/Vect.hpp"

namespace physics
{
  template<class First, class... It>
  auto getMinMaxImpl(std::tuple<First, It...> begin, std::tuple<First, It...> end)
  {
    auto copy(std::get<First>(begin));
    std::pair<claws::Vect<2u, decltype(*copy)>, claws::Vect<2u, decltype(*copy)>> start;

    if constexpr (!sizeof...(It))
      {
	start = *copy;
	++copy;
      }
    else
      start = getMinMaxImpl({std::get<It>(begin)...}, {std::get<It>(end)...});
    return std::accumulate(copy, std::get<First>(end), start, [](auto minmax, auto const &elem){
	for (std::size_t i(0ul); i != 2ul; ++i)
	  {
	    minmax.first[i] = std::min(minmax.first[i], elem.pos[i]);
	    minmax.second[i] = std::max(minmax.second[i], elem.pos[i]);
	  }
	return minmax;
      });
  }

  template<class... It, class Dest>
  void checkCollisionImpl(std::tuple<It...> begin, std::tuple<It...> end, Dest &&dest)
  {
    auto [min, max] = getMinMaxImpl(begin, end);
    claws::Vect<2u, double> mid((min + max) * 0.5);

    for (std::size_t i(0ul); i != 2ul; ++i)
      {
	auto isBelow([mid, i](auto const &a){
	      return a->pos[i] + a->radius > a->mid[i];
	  });
	auto isAbove([mid, i](auto const &a){
	      return a->pos[i] - a->radius < a->mid[i];
	  });
	for (auto pred : {isBelow, isAbove})
	  checkCollisionImpl(begin, {std::partition(std::get<It>(begin), std::get<It>(end), pred)...}, dest);
      }
  }

  template<class... T, class Dest>
  void prepareCollisionCheck(std::tuple<T...> begin, std::tuple<T...> end, Dest &&dest)
  {
    std::tuple<std::vector<decltype(&*std::get<T>(begin))>...> ptrStorage;

    int a[] = {(std::get<std::vector<decltype(&*std::get<T>(begin))>>(ptrStorage).resize(std::get<T>(end) - std::get<T>(begin)), 0)...};
    int b[] = {(std::transform(std::get<T>(begin), std::get<T>(end), std::get<std::vector<decltype(&*std::get<T>(begin))>>(ptrStorage).begin(), [](auto &a){return &a;}), 0)...};
    checkCollisionImpl({std::get<std::vector<decltype(&*std::get<T>(begin))>>(ptrStorage).begin()...},
		       {std::get<std::vector<decltype(&*std::get<T>(begin))>>(ptrStorage).end()...},
		       dest);
  }

  template<class It1, class It2, class Dest>
  void checkCollision(It1 begin1, It1 end1, It2 begin2, It2 end2, Dest &&dest)
  {
    checkCollisionImpl({begin1, begin2}, {end1, end2}, dest);
  }
};
