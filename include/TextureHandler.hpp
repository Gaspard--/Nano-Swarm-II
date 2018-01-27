#ifndef TEXTUREHANDLER_HPP_
# define TEXTUREHANDLER_HPP_

# include <map>
# include <memory>
# include "my_opengl.hpp"

class TextureHandler
{
public :
  TextureHandler();
  ~TextureHandler() = default;

  enum class TextureList : std::size_t
    {
      BOYAUX,
	PLANET,
	SWORDRADIUS,
	BULLET,
	ZOMBIE,
	HUMAN,
	PLAYER,
	TEST,
	NONE,
	HOUSE1,
	HOUSE2,
	HOUSE3,
	BOMB,
	BOMBHOLLOW,
	ROAD,
	BORDER,
	SLASH,
	SLASH2,
	SHURIKEN,
	BARFRONT,
	BARBACK,
	SPIN,
	SPINHOLLOW,
	BOMB_SPRITE,
	EXPLOSION,
	ARROW,
	TUTO,
	STARTPAGE,
	BLOOD,
	FLESH_SPRITE,
	GRASS,
	ZOMBIEHEAD,
	HUMANHEAD,
	SAVE_ME,
	HIGH_FIVE,
	BATTERY,
	UNIT,
	WORKER,
	BRUTE,
	SHOOTER,
	BOMBER,
	};

  static TextureHandler& getInstance();

  void addTexture(TextureList, std::string const&);
  Texture getTexture(TextureList) const;

private:

  static std::unique_ptr<TextureHandler> _instance;
  std::map<TextureList, Texture> _textures;
};

#endif /* !TEXTUREHANDLER_HPP_ */
