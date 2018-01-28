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
      BATTERY,
      UNIT,
      WORKER,
      BRUTE,
      BLADES,
      SHOOTER,
      BOMBER,
      PYLONE,
      countTexture
    };

  static TextureHandler& getInstance();

  void addTexture(TextureList, std::string const&);
  Texture getTexture(TextureList) const;
  const Texture& operator[](TextureList id) {
	  return _textures.at(id);
  };

private:

  static std::unique_ptr<TextureHandler> _instance;
  std::map<TextureList, Texture> _textures;
};

#endif /* !TEXTUREHANDLER_HPP_ */
