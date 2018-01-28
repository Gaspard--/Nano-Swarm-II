#pragma once

#include <utility>
#include <algorithm>
#include <numeric>
#include <tuple>
#include "claws/Vect.hpp"
#include <iostream>

template<class A>
struct TaggedIndex
{
  using Tag = A;
  unsigned short data;

  TaggedIndex<A>(unsigned short data = 0u)
    : data(data)
  {
  }

  TaggedIndex<A> &operator++() noexcept
  {
    ++data;
    return *this;
  }

  bool operator!=(TaggedIndex<A> const &other) const noexcept
  {
    return other.data != data;
  }

  bool operator==(TaggedIndex<A> const &other) const noexcept
  {
    return other.data == data;
  }
};

namespace std {
  template<class A>
  class hash<TaggedIndex<A>> {
  public:
    size_t operator()(TaggedIndex<A> const &c) const
    {
      return c.data;
    }
  };
}

template<class Solver, class Access>
struct Physics
{
  Solver &solver;
  Access &access;
private:
  using expander = int[];

  constexpr auto getMinMaxImpl(std::tuple<>, std::tuple<>) const noexcept
  {
    return std::pair<claws::Vect<2u, double>, claws::Vect<2u, double>>({std::numeric_limits<double>::max(), std::numeric_limits<double>::max()},
								       {std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest()});
  }

  template<class First, class... It>
  auto getMinMaxImpl(std::tuple<First, It...> const &begin, std::tuple<First, It...> const &end)
  {
    return std::accumulate(std::get<First>(begin), std::get<First>(end), getMinMaxImpl(std::tuple<It...>{std::get<It>(begin)...}, std::tuple<It...>{std::get<It>(end)...}),
			   [this](auto minmax, auto const &elem)
			   {
			     for (std::size_t i(0ul); i != 2ul; ++i)
			       {
				 minmax.first[i] = std::min(minmax.first[i], access[elem].fixture.pos[i]);
				 minmax.second[i] = std::max(minmax.second[i], access[elem].fixture.pos[i]);
			       }
			     return minmax;
			   });
  }

  template<class... It>
  void classicSolve(std::tuple<It...> const &begin, std::tuple<It...> const &end)
  {
    auto checkOthers([this](auto begin, auto end, std::tuple<It...> otherBegins, std::tuple<It...> otherEnds)
		     {
		       auto check([this](auto begin1, auto end1, auto begin2, auto end2)
				  {
				    for (auto it1(begin1); it1 < end1; ++it1)
				      for (auto it2(begin2); it2 < end2; ++it2)
					if ((access[*it1].fixture.pos - access[*it2].fixture.pos).length2() <
					    access[*it1].fixture.getRadius() * access[*it1].fixture.getRadius() +
					    access[*it2].fixture.getRadius() * access[*it2].fixture.getRadius())
					  solver(*it1, *it2);
				  });
		       (void)expander{(check(begin, end, std::get<It>(otherBegins), std::get<It>(otherEnds)), 0)...};
		     });
    (void)expander{(checkOthers(std::get<It>(begin), std::get<It>(end), {std::get<It>(begin)...},  {std::get<It>(end)...}), 0)...};
  }

  template<class... It>
  void checkCollisionImpl(std::tuple<It...> const &begin, std::tuple<It...> const &end, std::size_t level)
  {
    if (claws::Vect<sizeof...(It), std::size_t>{static_cast<std::size_t>((std::get<It>(end) - std::get<It>(begin)))...}.sum() < 10ul || level >= 4u)
      return classicSolve(begin, end);
    claws::Vect<2u, double> min;
    claws::Vect<2u, double> max;
    std::tie(min, max) = getMinMaxImpl(begin, end);
    claws::Vect<2u, double> mid((min + max) * 0.5);
    for (std::size_t i(0ul); i != 2ul; ++i)
      {
	auto isBelow([this, mid, i](auto const &a){
	    return access[a].fixture.pos[i] + access[a].fixture.getRadius() > mid[i];
	  });
	auto isAbove([this, mid, i](auto const &a){
	    return access[a].fixture.pos[i] - access[a].fixture.getRadius() < mid[i];
	  });
	checkCollisionImpl(begin, std::tuple<It...>{std::partition(std::get<It>(begin), std::get<It>(end), isBelow)...}, level + 1);
	checkCollisionImpl(begin, std::tuple<It...>{std::partition(std::get<It>(begin), std::get<It>(end), isAbove)...}, level + 1);
      }
  }

  template<class... T>
  void prepareCollisionCheck(std::tuple<T...> &containers)
  {
    (void)expander{(std::iota(std::get<T>(containers).begin(),
			      std::get<T>(containers).end(),
			      static_cast<unsigned short>(0u)), 0)...};
    checkCollisionImpl(std::tuple<typename T::iterator...>{std::get<T>(containers).begin()...},
		       std::tuple<typename T::iterator...>{std::get<T>(containers).end()...},
		       0);
  }

public:
  template<class... T>
  void checkCollision(std::vector<T> &... container)
  {
    std::tuple<std::vector<TaggedIndex<T>>...> containers;

    (void)expander{(std::get<std::vector<TaggedIndex<T>>>(containers).resize(container.size()), 0)...};
    prepareCollisionCheck(containers);
  }
};

template<class Solver, class Access>
auto makePhysics(Solver &solver, Access access)
{
  return Physics<Solver, Access>{solver, access};
}
