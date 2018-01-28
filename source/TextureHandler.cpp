#include "TextureHandler.hpp"

TextureHandler::TextureHandler()
{
  // Already loaded in display.cpp
  // _instance->addTexture(PLANET, "resources/planet.bmp");
  addTexture(TextureList::BATTERY, "resources/Battery.bmp");
  addTexture(TextureList::UNIT, "resources/Unit.bmp");
  addTexture(TextureList::WORKER, "resources/Worker.bmp");
  addTexture(TextureList::BRUTE, "resources/Brute.bmp");
  addTexture(TextureList::BLADES, "resources/Blades.bmp");
  addTexture(TextureList::SHOOTER, "resources/Shooter.bmp");
  addTexture(TextureList::BOMBER, "resources/Bomber.bmp");
  addTexture(TextureList::PYLONE, "resources/Pylone.bmp");
  addTexture(TextureList::SHOOTER_ENEMY, "resources/ShooterEnemy.bmp");
  addTexture(TextureList::BRUTE_ENEMY, "resources/BruteEnemy.bmp");
  addTexture(TextureList::BOMBER_ENEMY, "resources/BomberEnemy.bmp");
  addTexture(TextureList::BLADES_ENEMY, "resources/BladesEnemy.bmp");
  addTexture(TextureList::BATTERY_ENEMY, "resources/BatteryEnemy.bmp");
}

void TextureHandler::addTexture(TextureList id, std::string const& path)
{
  _textures[id] = my_opengl::loadTexture(path);
}

Texture TextureHandler::getTexture(TextureList id) const
{
  return (_textures.at(id));
}
