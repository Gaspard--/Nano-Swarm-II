#include "Laser.hpp"
#include "NanoBot.hpp"
#include "Logic.hpp"

NanoBot::NanoBot(Type type)
  : type(type)
{
}

void NanoBot::shooterAction(CollisionContainer &nearBots, Logic &logic)
{
  static constexpr double attackRange = 0.05;
  static constexpr double cooldown = 300;

  for (auto &entity : nearBots[this])
    {
      if (std::sqrt((fixture.pos - entity->fixture.pos).length2()) < attackRange
	  && !entity->dead)
	{
	  logic.addLaser(fixture.pos, entity->fixture.pos, 1.0);
	  this->cooldown = cooldown;
	  entity->dead = true;
	  return;
	}
    }
}
void NanoBot::workerAction(CollisionContainer &nearBots, Logic &logic)
{
  static constexpr double collectRange = 0.01;
  static constexpr double cooldown = 180;

  for (auto &entity : nearBots[this])
    {

    }
}

void NanoBot::bruteAction(CollisionContainer &nearBots, Logic &logic)
{
  static constexpr double attackRange = 0.01;
  static constexpr double cooldown = 60;

  for (auto &entity : nearBots[this])
    {
      if (std::sqrt((fixture.pos - entity->fixture.pos).length2()) < attackRange
	  && !entity->dead)
	{
	  this->cooldown = cooldown;
	  entity->dead = true;
	}
    }
}

void NanoBot::bomberAction(CollisionContainer &nearBots, Logic &logic)
{
  static constexpr double attackRange = 0.01;
  static constexpr double explosionRange = 0.06;

  for (auto &entity : nearBots[this])
    {
      if (std::sqrt((fixture.pos - entity->fixture.pos).length2()) < attackRange
	  && !entity->dead)
	{
	  for (auto &toKill : nearBots[this])
	    {
	      if (std::sqrt((fixture.pos - entity->fixture.pos).length2()) < explosionRange
		  && !toKill->dead)
		toKill->dead = true;
	    }
	  this->dead = true;
	  return;
	}
    }
}

TextureHandler::TextureList NanoBot::getTexture() const noexcept
{
  switch (type)
    {
    case Type::WORKER:
      return TextureHandler::TextureList::WORKER;
    case Type::BRUTE:
      return TextureHandler::TextureList::BRUTE;
    case Type::SHOOTER:
      return TextureHandler::TextureList::SHOOTER;
    case Type::BOMBER:
      return TextureHandler::TextureList::BOMBER;
    default:
      return TextureHandler::TextureList::UNIT;
    }
  return TextureHandler::TextureList::UNIT;
}
