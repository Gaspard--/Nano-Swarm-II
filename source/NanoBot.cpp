#include "Laser.hpp"
#include "NanoBot.hpp"
#include "Logic.hpp"

void NanoBot::update()
{
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
