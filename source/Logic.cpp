#include <thread>
#include <mutex>
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
  combo = 0;
  multiplier = 0;
}

void Logic::update()
{
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
  score += static_cast<int>(combo * add * (multiplier == 0 ? 1 : multiplier));
}

void Logic::incCombo()
{
  combo++;
}

void Logic::resetCombo()
{
  combo = 0;
}

long unsigned int Logic::getScore(void) const
{
  return score;
}

std::string Logic::getCombo(void) const
{
  if (combo < 2)
    return "";
  return "x" + std::to_string(combo);
}

EntityManager Logic::getEntityManager(void) const
{
  return entityManager;
}

std::string Logic::getTime(void) const
{
  auto secondTime((time * getTickTime().count()) / 1000000);
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
  if (display.isKeyPressed(GLFW_KEY_SPACE))
    selectAllBots();
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


void Logic::selectRect(Vect<2u, double> start, Vect<2u, double> end, Vect<4u, bool> keyPressed)
{
  std::vector<Entities> allUnits;
  // selectedBots.clear();
  allUnits.insert(allUnits.end(), em.allies.units.begin(), em.alliers.units.end());
  allUnits.insert(allUnits.end(), em.allies.batteries.begin(), em.allies.batteries.end());
  std::for_each(em.allies.units.begin(), nanoBots.end(), [this, start, end, keyPressed](NanoBot *bot){
      if (bot->isAlly())
	{
	  if (bot->getPos().x() >= start.x() && bot->getPos().x() <= end.x() &&
	      bot->getPos().y() >= start.y() && bot->getPos().y() <= end.y() &&
	      (keyPressed == Vect<4u, bool>(false, false, false, false) || keyPressed[bot->getType()]))
	    {
	      bot->setSelection(true);
	      selectedBots.push_back(bot);
	    }
	  else
	    bot->setSelection(false);
	}
    });
}
