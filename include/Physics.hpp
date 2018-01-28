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

  constexpr auto getMinMaxImpl(std::tuple<>, std::tuple<>, std::size_t) const noexcept
  {
    return std::pair<double, double>({std::numeric_limits<double>::max()},
				     {std::numeric_limits<double>::lowest()});
  }

  template<class First, class... It>
  auto getMinMaxImpl(std::tuple<First, It...> const &begin, std::tuple<First, It...> const &end, std::size_t dir)
  {
    return std::accumulate(std::get<First>(begin), std::get<First>(end), getMinMaxImpl(std::tuple<It...>{std::get<It>(begin)...}, std::tuple<It...>{std::get<It>(end)...}, dir),
			   [this, dir](auto minmax, auto const &elem)
			   {
			     minmax.first = std::min(minmax.first, access[elem].fixture.pos[dir]);
			     minmax.second = std::max(minmax.second, access[elem].fixture.pos[dir]);
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
  void checkCollisionImpl(std::tuple<It...> const &begin, std::tuple<It...> const &end, std::size_t level, std::size_t prevSize, std::size_t dir)
  {
    auto size(claws::Vect<sizeof...(It), std::size_t>{static_cast<std::size_t>((std::get<It>(end) - std::get<It>(begin)))...}.sum());
    if (size < 15ul || size * 16 > prevSize * 10 || level >= 12u)
      return classicSolve(begin, end); // n * n
    double min;
    double max;
    std::tie(min, max) = getMinMaxImpl(begin, end, dir); // n
    auto mid((min + max) * 0.5);
    auto isBelow([this, mid, dir](auto const &a) {
	return access[a].fixture.pos[dir] + access[a].fixture.getRadius() > mid;
      });
    auto isAbove([this, mid, dir](auto const &a) {
	return access[a].fixture.pos[dir] - access[a].fixture.getRadius() < mid;
      });
    checkCollisionImpl(begin, std::tuple<It...>{std::partition(std::get<It>(begin), std::get<It>(end), isBelow)...}, level + 1, size, !dir); // n
    checkCollisionImpl(begin, std::tuple<It...>{std::partition(std::get<It>(begin), std::get<It>(end), isAbove)...}, level + 1, size, !dir); // n
  }

  template<class... T>
  void prepareCollisionCheck(std::tuple<T...> &containers)
  {
    (void)expander{(std::iota(std::get<T>(containers).begin(),
			      std::get<T>(containers).end(),
			      static_cast<unsigned short>(0u)), 0)...};
    checkCollisionImpl(std::tuple<typename T::iterator...>{std::get<T>(containers).begin()...},
		       std::tuple<typename T::iterator...>{std::get<T>(containers).end()...},
		       0, ~0ul, 0ul);
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
