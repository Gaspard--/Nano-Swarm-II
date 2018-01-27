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
