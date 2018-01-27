#include <thread>
#include <mutex>
#include <set>
#include <unordered_set>

#include "Logic.hpp"
#include "Input.hpp"
#include "Display.hpp"

Logic::Logic(bool animation)
  : running(true),
    mousePos{0.0, 0.0},
    lastUpdate(Clock::now())
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
	entityManager.allies.units[i * 10 + j].fixture.pos = {0.1 * static_cast<double>(i) - 0.5, 0.1 * static_cast<double>(j) - 0.5};
	entityManager.allies.units[i * 10 + j].fixture.speed = {0.0, 0.0};
      }
}

template<class... T>
struct Collisions
{
  template<class U>
  using Set = std::set<U *>;

  template<class U>
  using Map = std::unordered_map<U *, std::tuple<Set<T>...>>;

  using Container = std::tuple<Map<T>...>;
};

void Logic::update()
{
  using Collisions = Collisions<TeamEntity<NanoBot, true>, TeamEntity<NanoBot, false>, TeamEntity<Battery, true>,  TeamEntity<Battery, false>, Battery>;

  Collisions::Container container;

  auto submitCollision([&container](auto &a, auto &b)
		       {
			 using A = std::remove_reference_t<decltype(a)>;
			 using B = std::remove_reference_t<decltype(b)>;

			 std::get<Collisions::Set<B>>(std::get<Collisions::Map<A>>(container)[&a]).emplace(&b);
		       });
  makePhysics(submitCollision).checkCollision(entityManager.allies.units, entityManager.allies.batteries,
					      entityManager.ennemies.units, entityManager.ennemies.batteries,
					      entityManager.pylones);
  auto updateEntity([&container](auto &unit)
  		    {
		      constexpr double const maxAccel(0.002);
		      using UnitType = std::remove_reference_t<decltype(unit)>;

		      auto &near(std::get<Collisions::Map<UnitType>>(container)[&unit]);
		      claws::Vect<2u, double> dir{0.0, 0.0};

		      auto reactToTeam([&dir, &unit](auto &container)
				       {
					 for (auto *ally : container)
					   {
					     claws::Vect<2u, double> posDelta(unit.fixture.pos - ally->fixture.pos);
					     claws::Vect<2u, double> speedDelta(unit.fixture.speed - ally->fixture.speed);
					     double coef = posDelta.scalar(posDelta) - 0.01;
					     
					     coef = coef > 0.01 ? 1 / coef : coef;
					     dir += speedDelta * 0.005;
					     dir += posDelta * 0.0001;
					   }
				       });
		      reactToTeam(std::get<Collisions::Set<TeamEntity<NanoBot, UnitType::getTeam()>>>(near));
		      reactToTeam(std::get<Collisions::Set<TeamEntity<Battery, UnitType::getTeam()>>>(near));
		      reactToTeam(std::get<Collisions::Set<Battery>>(near));
		      if (dir.length2() > maxAccel * maxAccel)
			dir = dir.normalized() * maxAccel;
		      unit.fixture.speed += dir;
  		      unit.fixture.pos += unit.fixture.speed;
  		    });

  entityManager.allies.iterOnTeam(updateEntity);
  entityManager.ennemies.iterOnTeam(updateEntity);  
}

  void Logic::tick(std::mutex &lock)
  {
    auto const now(Clock::now());

    if (now > lastUpdate + getTickTime() * 2)
      {
	lastUpdate = now;
	return ;
      }
    lastUpdate += getTickTime();
    if (now < lastUpdate)
      std::this_thread::sleep_for(lastUpdate - now);

    {
      std::lock_guard<std::mutex> scopedLock(lock);
      update();
    }
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
}

void Logic::handleMouse(Display const &display, GLFWwindow *, Mouse mouse)
{
  claws::Vect<2u, float> const size(display.getSize());

  mousePos = {mouse.x, mouse.y};
  mousePos -= claws::Vect<2u, double>(size[0] - size[1], 0.0);
  mousePos /= claws::Vect<2u, double>(size[1], -size[1]);
  mousePos += claws::Vect<2u, double>(-1.0, 1.0);
}

claws::Vect<2u, double> Logic::getMouse(Display const &display) const
{
  return display.getCamera().unapply(mousePos);
}

void Logic::handleButton(GLFWwindow *, Button button)
{
  if (button.button != GLFW_MOUSE_BUTTON_LEFT || button.action != GLFW_PRESS || gameOver)
    return ;
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
