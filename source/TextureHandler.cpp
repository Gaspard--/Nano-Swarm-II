#include "TextureHandler.hpp"

TextureHandler::TextureHandler()
{
  // Already loaded in display.cpp
  // _instance->addTexture(PLANET, "resources/planet.bmp");
  addTexture(TextureList::BOYAUX, "resources/boyaux.bmp");
  addTexture(TextureList::SWORDRADIUS, "resources/swordRadius.bmp");
  addTexture(TextureList::BULLET, "resources/bite.bmp");
  addTexture(TextureList::ZOMBIE, "resources/ZombieSpriteSheet.bmp");
  addTexture(TextureList::PLAYER, "resources/PlayerSpriteSheet.bmp");
  addTexture(TextureList::HUMAN, "resources/CitizenSpriteSheet.bmp");
  addTexture(TextureList::GRASS, "resources/grass.bmp");
  addTexture(TextureList::NONE, "resources/grass.bmp");
  addTexture(TextureList::HOUSE1, "resources/Shed.bmp");
  addTexture(TextureList::HOUSE2, "resources/House.bmp");
  addTexture(TextureList::HOUSE3, "resources/Mansion.bmp");
  addTexture(TextureList::BORDER, "resources/Border.bmp");
  addTexture(TextureList::TEST, "resources/test.bmp");
  addTexture(TextureList::BOMB, "resources/Bomb.bmp");
  addTexture(TextureList::BOMBHOLLOW, "resources/BombHollow.bmp");
  addTexture(TextureList::SLASH, "resources/SlashSpriteSheet.bmp");
  addTexture(TextureList::SLASH2, "resources/Slash2SpriteSheet.bmp");
  addTexture(TextureList::SHURIKEN, "resources/ShurikenSpriteSheet.bmp");
  addTexture(TextureList::BARFRONT, "resources/BarFront.bmp");
  addTexture(TextureList::BARBACK, "resources/BarBack.bmp");
  addTexture(TextureList::SPIN, "resources/Spin.bmp");
  addTexture(TextureList::SPINHOLLOW, "resources/SpinHollow.bmp");
  addTexture(TextureList::BOMB_SPRITE, "resources/BombSpriteSheet.bmp");
  addTexture(TextureList::EXPLOSION, "resources/ExplosionSpriteSheet.bmp");
  addTexture(TextureList::ARROW, "resources/arrow.bmp");
  addTexture(TextureList::BLOOD, "resources/BloodSplaterSpriteSheet.bmp");
  addTexture(TextureList::TUTO, "resources/Tuto.bmp");
  addTexture(TextureList::STARTPAGE, "resources/StartPage.bmp");
  addTexture(TextureList::FLESH_SPRITE, "resources/GutsSpriteSheet.bmp");
  addTexture(TextureList::ZOMBIEHEAD, "resources/ZombieHead.bmp");
  addTexture(TextureList::HUMANHEAD, "resources/CitizenHead.bmp");
  addTexture(TextureList::SAVE_ME, "resources/SaveMe.bmp");
  addTexture(TextureList::HIGH_FIVE, "resources/HighFive.bmp");
  addTexture(TextureList::BATTERY, "resources/Battery.bmp");
  addTexture(TextureList::UNIT, "resources/Unit.bmp");
}

void TextureHandler::addTexture(TextureList id, std::string const& path)
{
  _textures[id] = my_opengl::loadTexture(path);
}

Texture TextureHandler::getTexture(TextureList id) const
{
  return (_textures.at(id));
}
