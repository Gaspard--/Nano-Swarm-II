#include <thread>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <bitset>
#include "Logic.hpp"
#include "Input.hpp"
#include "Display.hpp"

Logic::Logic(bool animation)
  : lastUpdate(Clock::now()),
    running(true),
    mousePos{0.0, 0.0},
    dragOrigin{0.0, 0.0},
    leftClick(false),
    rightClick(false)
{
  time = 0;
  score = 0;
  restart = false;
  gameOver = false;
  multiplier = 0;
  for (std::size_t i(0ul); i < 10ul; ++i)
    for (std::size_t j(0ul); j < 10ul; ++j)
      {
	entityManager.allies.units.emplace_back(NanoBot::Type::BRUTE);
	entityManager.allies.units[i * 10 + j].fixture.pos = {0.045 * static_cast<double>(i) - 0.5, 0.045 * static_cast<double>(j) - 0.5};
	entityManager.allies.units[i * 10 + j].fixture.speed = {0.0, 0.0};
	entityManager.allies.units[i * 10 + j].fixture.target = entityManager.allies.units[i * 5 + j].fixture.pos * 0.5;
      }
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
      data.resize(index.data + 1);
    data[index.data] = true;
  }

  void clear()
  {
    data.clear();
  }
};

template<class... T>
struct Collisions
{
  template<class U>
  using Set = BitBasedSet<U>;

  template<class U>
  using Map = std::unordered_map<TaggedIndex<U>, std::tuple<Set<T>...>>;

  using Container = std::tuple<Map<T>...>;
};

template<class ...T, class Func>
static void forEachTuple(std::tuple<T...> &tuple, Func func)
{
  using expander = int[];

  (void)expander{(func(std::get<T>(tuple)), 0)...};
}

template<class T>
static void clearTuple(T &tuple)
{
  forEachTuple(tuple, [](auto &map)
	       {
		 for (auto &pair : map)
		   forEachTuple(pair.second, [](auto &set)
				{
				  set.clear();
				  // set.reserve(10);
				});
	       });
}

void Logic::update()
{
  struct Access
  {
    Logic &logic;

    auto &operator[](TaggedIndex<TeamEntity<NanoBot, true>> index)
    {
      return logic.entityManager.allies.units[index.data];
    }

    auto &operator[](TaggedIndex<TeamEntity<Battery, true>> index)
    {
      return logic.entityManager.allies.batteries[index.data];
    }

    auto &operator[](TaggedIndex<TeamEntity<NanoBot, false>> index)
    {
      return logic.entityManager.ennemies.units[index.data];
    }

    auto &operator[](TaggedIndex<TeamEntity<Battery, false>> index)
    {
      return logic.entityManager.ennemies.batteries[index.data];
    }

    auto &operator[](TaggedIndex<Battery> index)
    {
      return logic.entityManager.pylones[index.data];
    }
  };

  using Collisions = Collisions<TeamEntity<NanoBot, true>, TeamEntity<NanoBot, false>, TeamEntity<Battery, true>,  TeamEntity<Battery, false>, Battery>;

  static Collisions::Container container{};
  clearTuple(container);
  auto submitCollision([&container](auto a, auto b)
		       {
			 using A = decltype(a);
			 using B = decltype(b);

			 std::get<Collisions::Set<typename B::Tag>>(std::get<Collisions::Map<typename A::Tag>>(container)[a]).emplace(b);
		       });
  Access access{*this};
  makePhysics(submitCollision, access).checkCollision(entityManager.allies.units, entityManager.allies.batteries,
							     entityManager.ennemies.units, entityManager.ennemies.batteries,
							     entityManager.pylones);
  auto updateEntity([this, &container, &access](auto index)
  		    {
		      auto &unit(access[index]);
		      {
			constexpr double const maxAccel = 0.001;
			claws::Vect<2u, double> delta(unit.fixture.target - unit.fixture.pos);

			if (delta.length2() > maxAccel * maxAccel)
			  delta = delta.normalized() * maxAccel;
			unit.fixture.speed += delta;
		      }
		      {
			constexpr double const maxAccel(0.0001);
			claws::Vect<2u, double> dir{0.0, 0.0};
			using UnitType = std::remove_reference_t<decltype(unit)>;

			auto &near(std::get<Collisions::Map<UnitType>>(container)[index]);
			auto reactToTeam([&dir, &unit, &access](auto &container)
					 {
					   for (auto allyIndex : container)
					     {
					       auto &ally(access[allyIndex]);

					       claws::Vect<2u, double> posDelta(unit.fixture.pos - ally.fixture.pos);
					       claws::Vect<2u, double> speedDelta(unit.fixture.speed - ally.fixture.speed);
					       double coef = posDelta.length2() + 0.001;

					       coef = coef > 0.02 ? 1.0 / coef : coef;
					       dir += speedDelta * 0.005;
					       dir += posDelta * 0.0001;
					     }
					 });
			reactToTeam(std::get<Collisions::Set<TeamEntity<NanoBot, UnitType::getTeam()>>>(near));
			reactToTeam(std::get<Collisions::Set<TeamEntity<Battery, UnitType::getTeam()>>>(near));
			reactToTeam(std::get<Collisions::Set<Battery>>(near));
			if (dir.length2() > maxAccel * maxAccel)
			  dir = dir.normalized() * maxAccel;
			(unit.fixture.speed += dir) *= 0.9;
			unit.fixture.pos += unit.fixture.speed;
		      }
  		    });

  for (unsigned short i(0u); i < entityManager.allies.batteries.size(); ++i)
    updateEntity(TaggedIndex<TeamEntity<Battery, true>>(i));
  for (unsigned short i(0u); i < entityManager.allies.units.size(); ++i)
    updateEntity(TaggedIndex<TeamEntity<NanoBot, true>>(i));
  for (unsigned short i(0u); i < entityManager.ennemies.batteries.size(); ++i)
    updateEntity(TaggedIndex<TeamEntity<Battery, false>>(i));
  for (unsigned short i(0u); i < entityManager.ennemies.units.size(); ++i)
    updateEntity(TaggedIndex<TeamEntity<NanoBot, false>>(i));
}

void Logic::moveSelection(claws::Vect<2u, double> target)
{
  auto sumUnitPos([](auto averagePos, auto &unit) {
      if (unit.selected)
	{
	  ++averagePos.first;
	  averagePos.second += unit.fixture.pos;
	}
      return averagePos;
    });
  auto averagePosAndCount(std::accumulate(entityManager.allies.units.begin(), entityManager.allies.units.end(),
					  std::accumulate(entityManager.allies.batteries.begin(), entityManager.allies.batteries.end(),
						  std::pair<double, claws::Vect<2u, double>>{0.0, {0.0, 0.0}}, sumUnitPos), sumUnitPos));
  auto averagePos(averagePosAndCount.second / averagePosAndCount.first);
  auto update([target, &averagePos](auto &unit) {
      if (unit.selected)
	{
	  claws::Vect<2u, double> offset(unit.fixture.pos - averagePos);
	  unit.fixture.target = offset * 0.5 + target;
	}
    });
  entityManager.allies.iterOnTeam(update);
  // std::for_each(entityManager.allies.units.begin(), entityManager.allies.units.end(), update);
  // std::for_each(entityManager.allies.batteries.begin(), entityManager.allies.batteries.end(), update);
}

void Logic::tick(std::mutex &lock)
{
  auto const now(Clock::now());

  if (now > lastUpdate + getTickTime() * 2)
    {
      lastUpdate = now;
      return ;
    }
  {
    std::lock_guard<std::mutex> scopedLock(lock);
    update();
    lastUpdate += getTickTime();
  }
  if (now < lastUpdate)
    std::this_thread::sleep_for(lastUpdate - now);
}

void Logic::addToScore(int add)
{
  score += static_cast<int>(add * (multiplier == 0 ? 1 : multiplier));
}

std::string Logic::getScore(void) const
{
  return ("score = " + std::to_string(score));
}

EntityManager Logic::getEntityManager(void) const
{
  return entityManager;
}

std::string Logic::getTime(void) const
{
  std::size_t secondTime((time * getTickTime().count()) / 1000000ul);
  std::string   toReturn;

  if (secondTime / 60 >= 10)
    toReturn = std::to_string(secondTime / 60) + " m ";
  else if (secondTime / 60)
    toReturn = "0" + std::to_string(secondTime / 60) + " m ";
  if ((secondTime) % 60 >= 10)
    toReturn += std::to_string((secondTime) % 60) + " s";
  else
    toReturn += "0" + std::to_string((secondTime) % 60) + " s";
  return (toReturn);
}

void Logic::handleEvent(Display const &display, Event const& event)
{
  if (event)
    {
      switch (event.type)
        {
        case Event::KEY:
          handleKey(event.window, event.val.key);
          break;
        case Event::MOUSE:
          handleMouse(display, event.window, event.val.mouse);
          break;
        case Event::BUTTON:
          handleButton(event.window, event.val.button);
          break;
	default:
	  break;
        }
    }
}

void Logic::handleKey(GLFWwindow *window, Key key)
{
  switch (key.key)
    {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, true);
      break;
    default:
      break;
    }
}

void Logic::checkEvents(Display const &display)
{
  // if (display.isKeyPressed(GLFW_KEY_SPACE))
  //   selectAllBots();
}

void Logic::handleMouse(Display const &display, GLFWwindow *, Mouse mouse)
{
  claws::Vect<2u, float> const size(display.getSize());

  mousePos = {mouse.x, mouse.y};
  mousePos *= 2.0;
  mousePos -= claws::Vect<2u, double>(size[0] - size[1], 0.0);
  mousePos /= claws::Vect<2u, double>(size[1], -size[1]);
  mousePos += claws::Vect<2u, double>(-1.0, 1.0);
  // return display.getCamera().unapply(mousePos);
}

void Logic::handleButton(GLFWwindow *, Button button)
{
  // if (button.button != GLFW_MOUSE_BUTTON_LEFT || button.action != GLFW_PRESS || gameOver)
  //   return ;
  if (button.button == GLFW_MOUSE_BUTTON_LEFT)
    {
      if (button.action == GLFW_PRESS)
	{
	  dragOrigin = mousePos;
	  leftClick = true;
	}
      else
	{
	  selectBots();
	  leftClick = false;
	}
    }
  // RIGHT BUTTON
  else if (button.button == GLFW_MOUSE_BUTTON_RIGHT)
    {
      if (button.action == GLFW_PRESS)
	{
	  rightClick = true;
	  moveSelection(mousePos);
	}
      else
	{
	  rightClick = false;
	}
    }
}

claws::Vect<2, double> Logic::getPlayerPos(void) const
{
  return {0.0, 0.0};
}


bool Logic::getRestart(void) const
{
  return restart;
}

bool Logic::getGameOver(void) const
{
  return gameOver;
}

void Logic::selectBots()
{
  claws::Vect<2u, double> start(std::min(mousePos.x(), dragOrigin.x()), std::min(mousePos.y(), dragOrigin.y()));
  claws::Vect<2u, double> end(std::max(mousePos.x(), dragOrigin.x()), std::max(mousePos.y(), dragOrigin.y()));
  selectRect(start, end);
}

void Logic::selectRect(claws::Vect<2u, double> start, claws::Vect<2u, double> end)
{
  auto& allies = entityManager.allies;
  std::for_each(allies.units.begin(), allies.units.end(), [start, end](NanoBot& bot){
      if (bot.fixture.pos.x() >= start.x() && bot.fixture.pos.x() <= end.x() &&
	  bot.fixture.pos.y() >= start.y() && bot.fixture.pos.y() <= end.y())
	{
	  bot.setSelection(true);
	}
      else
	bot.setSelection(false);
    });
  std::for_each(allies.batteries.begin(), allies.batteries.end(), [start, end](Battery& battery){
      if (battery.fixture.pos.x() >= start.x() && battery.fixture.pos.x() <= end.x() &&
	  battery.fixture.pos.y() >= start.y() && battery.fixture.pos.y() <= end.y())
	{
	  battery.setSelection(true);
	}
      else
	battery.setSelection(false);
    });
}
