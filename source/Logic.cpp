#include <thread>
#include <mutex>
#include <type_traits>
#include <bitset>
#include <algorithm>
#include "Logic.hpp"
#include "Input.hpp"
#include "Display.hpp"
#include "Camera.hpp"

Logic::Logic(bool animation)
  : lastUpdate(Clock::now()),
    running(true),
    mousePos{0.0, 0.0},
    dragOrigin{0.0, 0.0},
    leftClick(false),
    rightClick(false),
    selectedTypes{{false, false, false, false, false}}
{
  timer = 0;
  score = 0;
  restart = false;
  gameOver = false;
  multiplier = 0;
  level = 0;
  spawnDelay = 0;
  for (std::size_t i(0ul); i < 10ul; ++i)
    for (std::size_t j(0ul); j < 10ul; ++j)
      {
	entityManager.allies.units.emplace_back((i & 1) ? NanoBot::Type::BRUTE :  NanoBot::Type::SHOOTER);
	entityManager.allies.units[i * 10 + j].fixture.pos = {0.045 * static_cast<double>(i) - 0.5, 0.045 * static_cast<double>(j) - 0.5};
	entityManager.allies.units[i * 10 + j].fixture.speed = {0.0, 0.0};
	entityManager.allies.units[i * 10 + j].fixture.target = entityManager.allies.units[i * 10 + j].fixture.pos;
      }
  for (std::size_t i(0ul); i < 10ul; ++i)
    for (std::size_t j(0ul); j < 10ul; ++j)
      {
	entityManager.allies.batteries.emplace_back(10);
	entityManager.allies.batteries[i * 10 + j].fixture.pos = {0.045 * static_cast<double>(i), 0.045 * static_cast<double>(j) + 1.0};
	entityManager.allies.batteries[i * 10 + j].fixture.speed = {0.0, 0.0};
	entityManager.allies.batteries[i * 10 + j].fixture.target = entityManager.allies.batteries[i * 10 + j].fixture.pos;
      }
  createPylone(claws::Vect<2u, double>{0.045 - 0.3, 0.045 - 1.0});
}

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
				});
	       });
}

void Logic::update(Display &display)
{
  unsigned int tick = 0;
  if (++tick > 60)
    tick = 0;

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

  lines.resize(0);
  static Collisions::Container container{};
  clearTuple(container);
  auto submitCollision([](auto a, auto b)
		       {
			 using A = decltype(a);
			 using B = decltype(b);

			 std::get<Collisions::Set<typename B::Tag>>(std::get<Collisions::Map<typename A::Tag>>(container)[a]).emplace(b);
		       });
  Access access{*this};
  makePhysics(submitCollision, access).checkCollision(entityManager.allies.units, entityManager.allies.batteries,
						      entityManager.ennemies.units, entityManager.ennemies.batteries,
						      entityManager.pylones);


  for (auto &bot : entityManager.allies.units)
    bot.hasPlayed = false;
  for (auto &bot : entityManager.ennemies.units)
    bot.hasPlayed = false;
  for (unsigned short i(0u); i < entityManager.allies.batteries.size(); ++i)
    updateEntity(TaggedIndex<TeamEntity<Battery, true>>(i), entityManager.allies.batteries[i], container, access);
  for (unsigned short i(0u); i < entityManager.ennemies.batteries.size(); ++i)
    updateEntity(TaggedIndex<TeamEntity<Battery, false>>(i), entityManager.ennemies.batteries[i], container, access);
  for (unsigned short i(0u); i < entityManager.pylones.size(); ++i)
    {
      auto &nearEntities(std::get<Collisions::Map<Battery>>(container)[TaggedIndex<Battery>(i)]);
      for (auto allyIndex : std::get<Collisions::Set<TeamEntity<NanoBot, true>>>(nearEntities))
	if (!access[allyIndex].hasPlayed)
	  updateEntity(allyIndex, entityManager.pylones[i], container, access);
      for (auto allyIndex : std::get<Collisions::Set<TeamEntity<NanoBot, false>>>(nearEntities))
	if (!access[allyIndex].hasPlayed)
	  updateEntity(allyIndex, entityManager.pylones[i], container, access);
      for (auto allyIndex : std::get<Collisions::Set<TeamEntity<Battery, true>>>(nearEntities))
        access[allyIndex].reload(entityManager.pylones[i]);
      for (auto allyIndex : std::get<Collisions::Set<TeamEntity<Battery, false>>>(nearEntities))
        access[allyIndex].reload(entityManager.pylones[i]);
    }
  auto update([](Entity &unit){
      unit.fixture.speed *= 0.9;
      unit.fixture.pos += unit.fixture.speed;
    });
  entityManager.allies.iterOnTeam(update);
  entityManager.ennemies.iterOnTeam(update);
  checkEvents(display, tick);
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
  auto count(averagePosAndCount.first);

  auto update([target, &averagePos, count](auto &unit) {
      if (unit.selected)
	{
	  claws::Vect<2u, double> offset(unit.fixture.pos - averagePos);
	  unit.fixture.target = offset * (1.0 - (200.0 / (count + 200.0))) + target;
	}
    });
  entityManager.allies.iterOnTeam(update);
}

void Logic::tick(std::mutex &lock, Display& display)
{
  if (spawnDelay == 0)
    {
      ++level;
      spawnEnemies(display.getCamera());
      spawnDelay = 15000 / (level + 50);
    }
  else
    --spawnDelay;
  spawnPylone(display.getCamera());

  auto const now(Clock::now());

  if (now > lastUpdate + getTickTime() * 2)
    {
      lastUpdate = now;
      return ;
    }
  {
    std::lock_guard<std::mutex> scopedLock(lock);
    update(display);
    lastUpdate += getTickTime();
  }
  if (now < lastUpdate)
    std::this_thread::sleep_for(lastUpdate - now);
}

void Logic::addToScore(int add)
{
  score += static_cast<int>(add * (multiplier == 0 ? 1 : multiplier));
}

void Logic::addToTimer(unsigned int add)
{
  timer += add;
}

void Logic::createBot(claws::Vect<2u, double> pos, claws::Vect<2u, double> speed, bool ally, NanoBot::Type type)
{
  if (ally)
  {
    entityManager.allies.units.emplace_back(type);
	  entityManager.allies.units.back().fixture.pos = pos;
	  entityManager.allies.units.back().fixture.speed = speed;
	  entityManager.allies.units.back().fixture.target = entityManager.allies.units.back().fixture.pos;
  }
  else
  {
    entityManager.ennemies.units.emplace_back(type);
	  entityManager.ennemies.units.back().fixture.pos = pos;
	  entityManager.ennemies.units.back().fixture.speed = speed;
	  entityManager.ennemies.units.back().fixture.target = entityManager.ennemies.units.back().fixture.pos;
  }
}

void Logic::createBatterie(claws::Vect<2u, double> pos, claws::Vect<2u, double> speed, bool ally)
{
  if (ally)
  {
    entityManager.allies.batteries.emplace_back(10);
	  entityManager.allies.batteries.back().fixture.pos = pos;
	  entityManager.allies.batteries.back().fixture.speed = speed;
	  entityManager.allies.batteries.back().fixture.target = entityManager.allies.batteries.back().fixture.pos;
  }
  else
  {
    entityManager.ennemies.batteries.emplace_back(10);
	  entityManager.ennemies.batteries.back().fixture.pos = pos;
	  entityManager.ennemies.batteries.back().fixture.speed = speed;
	  entityManager.ennemies.batteries.back().fixture.target = entityManager.ennemies.batteries.back().fixture.pos;
  }
}

void Logic::createPylone(claws::Vect<2u, double> pos)
{
  entityManager.pylones.emplace_back(100);
	entityManager.pylones.back().fixture.pos = pos;
}

void Logic::spawnEnemies(Camera const &camera)
{
  claws::Vect<2u, double> spawnCenter((claws::Vect<2u, double>{sin(level * level), cos(level * level)} - camera.offset) * 3.0 / camera.zoom);

  //std::cout << "X: " << spawnCenter[0] << "        Y: " << spawnCenter[1] << std::endl;
  unsigned int i(0);
  for (i = 0; i < (10 + level) / 3; i++)
    createBot(claws::Vect<2u, double>{(i % 5) * 0.05, (i / 5) * 0.05} + spawnCenter,
              {0.0, 0.0},
	            false,
              NanoBot::Type::BRUTE);
  createBatterie(claws::Vect<2u, double>{(i % 5) * 0.05, (i / 5) * 0.05} + spawnCenter,
            {0.0, 0.0},
	          false);
}

void Logic::spawnPylone(Camera const &camera)
{
  if (entityManager.pylones.back().getPower() <= 10)
  {
    claws::Vect<2u, double> spawnCenter((claws::Vect<2u, double>{sin(level * level), cos(level * level)} - camera.offset) * 3.0 / camera.zoom);
    createPylone(claws::Vect<2u, double>{(1 % 5) * 0.05, (1 / 5) * 0.05} + spawnCenter);
  }
}

std::string Logic::getScore(void) const
{
  return ("Score = " + std::to_string(score));
}

EntityManager Logic::getEntityManager(void) const
{
  return entityManager;
}

std::string Logic::getTimer(void) const
{
  std::size_t secondTime((timer * getTickTime().count()) / 1000000ul);
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
          handleKey(display, event.window, event.val.key);
          break;
        case Event::MOUSE:
          handleMouse(display, event.window, event.val.mouse);
          break;
        case Event::BUTTON:
          handleButton(event.window, event.val.button, display);
          break;
	default:
	  break;
        }
    }
}

void Logic::handleKey(Display const& display, GLFWwindow *window, Key key)
{
  switch (key.key)
    {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, true);
      break;
    case GLFW_KEY_1 ... GLFW_KEY_5:
      if (key.action == GLFW_PRESS)
	{
	  selectedTypes[key.key - GLFW_KEY_1] = true;
	}
      else if (key.action == GLFW_RELEASE)
	{
	  selectedTypes[key.key - GLFW_KEY_1] = false;
	}
      if (leftClick)
      	selectBots(display);
      else
	{
	  refreshSelection();
	  selectType(selectedTypes);
	}
      break;
    default:
      break;
    }
}

void Logic::checkEvents(Display const &display, unsigned tick)
{
  if (rightClick)
    moveSelection(getMousePos(display));
  if (display.isKeyPressed(GLFW_KEY_SPACE))
    selectAllBots();
}

void Logic::handleMouse(Display const &display, GLFWwindow *, Mouse mouse)
{
  claws::Vect<2u, float> const size(display.getSize());

  mousePos = {mouse.x, mouse.y};
  mousePos *= 2.0;
  mousePos -= claws::Vect<2u, double>(size[0] - size[1], 0.0);
  mousePos /= claws::Vect<2u, double>(size[1], -size[1]);
  mousePos += claws::Vect<2u, double>(-1.0, 1.0);
}

void Logic::handleButton(GLFWwindow *, Button button, Display const &display)
{
  // if (button.button != GLFW_MOUSE_BUTTON_LEFT || button.action != GLFW_PRESS || gameOver)
  //   return ;
  if (button.button == GLFW_MOUSE_BUTTON_LEFT)
    {
      if (button.action == GLFW_PRESS)
	{
	  dragOrigin = getMousePos(display);
	  leftClick = true;
	}
      else
	{
	  selectBots(display);
	  leftClick = false;
	}
    }
  // RIGHT BUTTON
  else if (button.button == GLFW_MOUSE_BUTTON_RIGHT)
    {
      if (button.action == GLFW_PRESS)
	{
	  rightClick = true;
	}
      else
	{
	  rightClick = false;
	}
    }
}

claws::Vect<2, double> Logic::getMousePos(Display const &display) const
{
  return display.getCamera().unapply(mousePos);
}


bool Logic::getRestart(void) const
{
  return restart;
}

bool Logic::getGameOver(void) const
{
  return gameOver;
}

void Logic::refreshSelection()
{
  auto& allies = entityManager.allies;
  for (auto& bot : allies.units)
    {
      if (selectedTypes != std::array<bool, 5>{{false, false, false, false, false}} &&
	  !selectedTypes[bot.type])
	bot.setSelection(false);
    }
  for (auto& battery : allies.batteries)
    {
      if (selectedTypes != std::array<bool, 5>{{false, false, false, false, false}} &&
	  !selectedTypes[4])
	battery.setSelection(false);
    }
}

void Logic::selectType(std::array<bool, 5> const& types)
{
  auto& allies = entityManager.allies;
  bool noSelection = (find_if(allies.units.begin(), allies.units.end(),
			      [] (NanoBot& bot) { return bot.selected; }) == allies.units.end() &&
		      find_if(allies.batteries.begin(), allies.batteries.end(),
			      [] (Battery& battery) { return battery.selected; }) == allies.batteries.end());
  if (!noSelection)
    return;
  if (types != std::array<bool, 5>{{false, false, false, false, false}})
    {
      for (auto& bot : allies.units)
	{
	  if (types[bot.type])
	    bot.setSelection(true);
	}
      for (auto& battery : allies.batteries)
	{
	  if (types[4])
	    battery.setSelection(true);
	}
    }
}

void Logic::selectAllBots()
{
  auto& allies = entityManager.allies;
  for (auto& bot : allies.units)
    bot.setSelection(true);
  for (auto& battery : allies.batteries)
    battery.setSelection(true);
}

void Logic::selectBots(Display const &display)
{
  claws::Vect<2u, double> start(std::min(getMousePos(display).x(), dragOrigin.x()), std::min(getMousePos(display).y(), dragOrigin.y()));
  claws::Vect<2u, double> end(std::max(getMousePos(display).x(), dragOrigin.x()), std::max(getMousePos(display).y(), dragOrigin.y()));
  selectRect(start, end);
}

void Logic::selectRect(claws::Vect<2u, double> start, claws::Vect<2u, double> end)
{
  auto& allies = entityManager.allies;
  std::for_each(allies.units.begin(), allies.units.end(), [this, start, end](NanoBot& bot){
      if (bot.fixture.pos.x() >= start.x() && bot.fixture.pos.x() <= end.x() &&
	  bot.fixture.pos.y() >= start.y() && bot.fixture.pos.y() <= end.y() &&
	  (selectedTypes == std::array<bool, 5>{{false, false, false, false, false}} ||
	   selectedTypes[bot.type]))
	bot.setSelection(true);
      else
	bot.setSelection(false);
    });
  std::for_each(allies.batteries.begin(), allies.batteries.end(), [this, start, end](Battery& battery){
      if (battery.fixture.pos.x() >= start.x() && battery.fixture.pos.x() <= end.x() &&
	  battery.fixture.pos.y() >= start.y() && battery.fixture.pos.y() <= end.y() &&
	  (selectedTypes == std::array<bool, 5>{{false, false, false, false, false}} ||
	   selectedTypes[4]))
	battery.setSelection(true);
      else
	battery.setSelection(false);
    });
}
