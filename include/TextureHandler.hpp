#ifndef TEXTUREHANDLER_HPP_
# define TEXTUREHANDLER_HPP_

# include <map>
# include <memory>
# include "my_opengl.hpp"

struct TextureFull
{
  Texture texture;
  int frame;
};

class TextureHandler
{
public :
  TextureHandler();
  ~TextureHandler() = default;

  enum class TextureList : std::size_t
    {
      BATTERY,
      UNIT,
      WORKER,
      BRUTE,
      BLADES,
      SHOOTER,
      BOMBER,
      PYLONE,
      SHOOTER_ENEMY,
      BRUTE_ENEMY,
      WORKER_ENEMY,
      BOMBER_ENEMY,
      BLADES_ENEMY,
      BATTERY_ENEMY,
      countTexture
    };

  static TextureHandler& getInstance();

  void addTexture(TextureList, std::string const&);
  Texture getTexture(TextureList) const;
  TextureFull getTextureFull(TextureList) const;
  const Texture& operator[](TextureList id) {
	  return _textures.at(id);
  };

private:

  static std::unique_ptr<TextureHandler> _instance;
  std::map<TextureList, Texture> _textures;
  static constexpr int _getFrames(int id)
  {
    constexpr int frames[static_cast<int>(TextureList::countTexture)] =
      {
	2, // BATTERY
	1, // UNIT
	1, // WORKER
	5, // BRUTE
	1, // BLADES
	8, // SHOOTER
	1, // BOMBER
	1, // PYLONE
	8, // SHOOTER E
	5, // BRUTE E
	1, // BOMBER E
	1, // BLADES E
	2 // BATTERY E
      };
    return (frames[id]);
  }
};

#endif /* !TEXTUREHANDLER_HPP_ */
