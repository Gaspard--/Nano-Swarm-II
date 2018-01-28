#include "Laser.hpp"
#include "NanoBot.hpp"
#include "Logic.hpp"

void NanoBot::ia(EntityManager& em)
{
  // go near ennemies
  // if energy is available go closer to ennemy
  // shoot if there is energy close-by
}

void NanoBot::update()
{
}


void NanoBot::shooterAction(CollisionContainer &nearBots, Logic &logic)
{
  static constexpr double attackRange = 0.05;
  static constexpr unsigned cooldown = 300;

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

void NanoBot::workerAction(CollisionContainer &, Logic &)
{
  // static constexpr double collectRange = 0.01;
  // static constexpr unsigned cooldown = 180;

}

void NanoBot::bruteAction(CollisionContainer &nearBots, Logic &)
{
  static constexpr double attackRange = 0.01;
  static constexpr unsigned cooldown = 60;

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

void NanoBot::bomberAction(CollisionContainer &nearBots, Logic &)
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

TextureHandler::TextureList NanoBot::getTexture(bool team) const noexcept
{
  switch (type)
    {
    case Type::WORKER:
      return team ? TextureHandler::TextureList::WORKER : TextureHandler::TextureList::WORKER_ENEMY;
    case Type::BRUTE:
      if (cooldown > 50)
	return team ? TextureHandler::TextureList::BLADES : TextureHandler::TextureList::BLADES_ENEMY;
	  return team ? TextureHandler::TextureList::BRUTE : TextureHandler::TextureList::BRUTE_ENEMY;
    case Type::SHOOTER:
      return team ? TextureHandler::TextureList::SHOOTER : TextureHandler::TextureList::SHOOTER_ENEMY;
    case Type::BOMBER:
      return team ? TextureHandler::TextureList::BOMBER : TextureHandler::TextureList::BOMBER_ENEMY;
    default:
      return TextureHandler::TextureList::UNIT;
    }
  return TextureHandler::TextureList::UNIT;
}
