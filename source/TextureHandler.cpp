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
}

void TextureHandler::addTexture(TextureList id, std::string const& path)
{
  _textures[id] = my_opengl::loadTexture(path);
}

Texture TextureHandler::getTexture(TextureList id) const
{
  return (_textures.at(id));
}
