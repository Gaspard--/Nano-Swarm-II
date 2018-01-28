#ifndef LOGIC_HPP_
# define LOGIC_HPP_

# include <random>
# include <algorithm>
# include <vector>
# include <unordered_map>
# include <unordered_set>
# include <memory>
# include <chrono>
# include <mutex>
# include "Input.hpp"
# include "EntityManager.hpp"
# include "Laser.hpp"
# include "Camera.hpp"

class Display;
class Camera;

class Logic
{
public:
  using Clock = std::conditional<std::chrono::high_resolution_clock::is_steady,
                                 std::chrono::high_resolution_clock,
                                 std::chrono::steady_clock>::type;

  decltype(Clock::now()) lastUpdate;

  static constexpr std::chrono::microseconds const getTickTime()
  {
    return std::chrono::microseconds(1000000 / 60);
  };

  std::vector<std::pair<claws::Vect<2u, double>, claws::Vect<2u, double>>> lines;
private:
  EntityManager entityManager;

  unsigned int timer;
  unsigned long int score;
  double multiplier;

  bool restart;
  bool gameOver;
  bool tutoPage;
  bool startPage;
  bool running;
  std::vector<Laser> lasers;
  unsigned int spawnDelay;
  unsigned int level;

  // input variables
  claws::Vect<2u, double> mousePos;
  claws::Vect<2u, double> dragOrigin;
  // mouse variables
  bool leftClick;
  bool rightClick;
  // key press
  std::array<bool, 5> selectedTypes;

  std::size_t updatesSinceLastFrame;


  void handleKey(Display const &, GLFWwindow *window, Key key);
  void handleMouse(Display const &, GLFWwindow *window, Mouse mouse);
  void handleButton(GLFWwindow *window, Button button, Display const &);
  void selectBots(Display const &);
  void selectRect(claws::Vect<2u, double> start, claws::Vect<2u, double> end);
  void refreshSelection();
  void selectAllBots();
  void selectType(std::array<bool, 5> const& types);

public:
  Logic(bool animation = true);

  void handleEvent(Display const &, Event const& event);
  void checkEvents(Display const &, unsigned tick);
  void selectRect(claws::Vect<2u, double>, claws::Vect<2u, double>, claws::Vect<4u, bool>);
  void update(Display&);
  void moveSelection(claws::Vect<2u, double> target);
  void tick(std::mutex &lock, Display&);
  void addToScore(int);
  void addToTimer(unsigned int);
  void createBot(claws::Vect<2u, double> pos, claws::Vect<2u, double> speed, bool ally, NanoBot::Type type);
  void spawnEnemies(Camera const &camera);

  template<class... Args>
  void addLaser(Args &&...args)
  {
    lasers.emplace_back(std::forward<Args>(args)...);
  }

  EntityManager	getEntityManager(void) const;
  claws::Vect<2, double> getMousePos(Display const &) const;
  claws::Vect<2u, double> getMouse(Display const &) const;
  std::string	getTimer(void) const;
  std::string	getScore(void) const;
  bool          getGameOver(void) const;
  bool          getRestart(void) const;
  bool          getStartPage(void) const;
  bool          getTutoPage(void) const;

  bool		isRunning() const noexcept
  {
    return running;
  }

  bool		&isRunning() noexcept
  {
    return running;
  }

  template<class T>
  struct BitBasedSet
  {
    std::vector<bool> data;

    struct Iterator
    {
      std::vector<bool> &data;
      TaggedIndex<T> index;

      auto operator*()
      {
	return index;
      }

      auto operator++()
      {
	++index;
	while (index.data != data.size() && !data.at(index.data))
	  ++index;
	return *this;
      }

      auto operator==(Iterator const &other) const noexcept
      {
	return index == other.index;
      }

      auto operator!=(Iterator const &other) const noexcept
      {
	return index != other.index;
      }
    };

    auto begin()
    {
      if (data.empty() || data[0])
	return Iterator{data, {0}};
      return ++Iterator{data, {0}};
    }

    auto end()
    {
      return Iterator{data, {static_cast<unsigned short>(data.size())}};
    }

    void emplace(TaggedIndex<T> index)
    {
      if (data.size() <= index.data)
	data.resize(index.data + 1, 0);
      data[index.data] = true;
    }

    void clear()
    {
      data.assign(data.size(), 0);
    }
  };


  template<class... T>
  struct CollisionsImpl
  {
    template<class U>
    using Set = BitBasedSet<U>;

    template<class U>
    using Map = std::unordered_map<TaggedIndex<U>, std::tuple<Set<T>...>>;

    using Container = std::tuple<Map<T>...>;
  };

  using Collisions = CollisionsImpl<TeamEntity<NanoBot, true>, TeamEntity<NanoBot, false>, TeamEntity<Battery, true>,  TeamEntity<Battery, false>, Battery>;



  template<class Index, class Source, class Container, class Access>
  void updateEntity(Index index, Source &source, Container &container, Access &access)
  {
    auto &unit(access[index]);
    {
      constexpr double const maxAccel(0.0005);
      claws::Vect<2u, double> dir{0.0, 0.0};
      using UnitType = std::remove_reference_t<decltype(unit)>;

      auto &nearEntities(std::get<Collisions::Map<UnitType>>(container)[index]);
      auto reactToTeam([&dir, &unit, &access](auto &container)
		       {
			 for (auto allyIndex : container)
			   {
			     auto &ally(access[allyIndex]);

			     claws::Vect<2u, double> posDelta(unit.fixture.pos - ally.fixture.pos);
			     claws::Vect<2u, double> speedDelta(unit.fixture.speed - ally.fixture.speed);
			     double coef = posDelta.length2() - 0.02;

			     coef = coef > 0.02 ? 1.0 / coef : coef;
			     dir += speedDelta * 0.005;
			     dir += posDelta * 0.0001;
			   }
		       });
      reactToTeam(std::get<Collisions::Set<TeamEntity<Battery, UnitType::getTeam()>>>(nearEntities));
      reactToTeam(std::get<Collisions::Set<TeamEntity<NanoBot, UnitType::getTeam()>>>(nearEntities));
      // reactToTeam(std::get<Collisions::Set<Battery>>(nearEntities));
      for (auto allyIndex : std::get<Collisions::Set<TeamEntity<NanoBot, UnitType::getTeam()>>>(nearEntities))
	if (!access[allyIndex].hasPlayed)
	  {
	    access[allyIndex].hasPlayed = true;
	    lines.push_back({unit.fixture.pos, access[allyIndex].fixture.pos});
	    updateEntity(allyIndex, source, container, access);
	  }
      if (dir.length2() > maxAccel * maxAccel)
	dir = dir.normalized() * maxAccel;
      (unit.fixture.speed += dir);
    }
    {
      constexpr double const maxAccel = 0.001;
      claws::Vect<2u, double> delta(unit.fixture.target - unit.fixture.pos);

      if (delta.length2() > maxAccel)
	{
	  if (delta.length2() > maxAccel * maxAccel)
	    delta = delta.normalized() * maxAccel;
	  unit.fixture.speed += delta;
	}
    }
  }

};

#endif // !LOGIC_HPP_
