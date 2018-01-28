#pragma once

#define NOMINMAX
#include <memory>
#include <cmath>
#include <array>
#include "claws/Vect.hpp"
#include "RenderContext.hpp"
#include "my_opengl.hpp"
#include "FreeTypeLib.hpp"
#include "Rect.hpp"
#include "Logic.hpp"
#include "RenderTexture.hpp"
#include "Camera.hpp"
#include "Bind.hpp"
#include "Renderable.hpp"
#include "EntityRenderable.hpp"
#include "DisplayInfo.hpp"
#include "TextureHandler.hpp"

class Display
{
private:
  struct GlfwContext
  {
    GlfwContext();
    ~GlfwContext();
  } glfwContext;

  Camera camera;

  std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> window;
  FreeTypeLib fontHandler;
  RenderContext textureContext;
  glBuffer textureBuffer;
  RenderContext rectContext;
  glBuffer rectBuffer;
  RenderContext textContext;
  glBuffer textBuffer;
  Texture planet;
  Texture planetBackground;
  Texture background;
  Texture bloodSpray[3];
  Texture mobSpray[3];
  RenderTexture planetRenderTexture;
  claws::Vect<2u, float> size;
  claws::Vect<2u, float> dim;
  std::vector<HudBlock>	hud;

  DisplayInfo displayInfo;

  TextureHandler textureHandler;

public:
  Display();
  ~Display();

  GLFWwindow *getWindow() const;

  void displayText(std::string const &txt, unsigned int fontSize, claws::Vect<2u, float> step, claws::Vect<2u, float> textPos,  claws::Vect<2u, float> rotation, claws::Vect<3u, float> color);

  void displayRect(Rect const &);

  void displayHudBlock(const HudBlock &block);

  void displayInterface();

  void displayRenderableAsHUD(Renderable const& renderable, GLuint texture);

  claws::Vect<2u, float> getDim() const;

  claws::Vect<2u, float> getSize() const;

  Camera const &getCamera() const;

  void render();
  bool isRunning() const;
  bool isKeyPressed(int key) const;

  void copyRenderData(Logic const &);

  static claws::Vect<2u, float> rotate(claws::Vect<2u, float> a, claws::Vect<2u, float> b) noexcept
  {
    return {a[0] * b[0] - a[1] * b[1], a[0] * b[1] + a[1] * b[0]};
  }

  auto getCorner(Renderable const &renderable, claws::Vect<2u, float> corner) const noexcept
  {
    return renderable.destPos + ((corner - claws::Vect<2u, float>{0.5f, 0.0f}) * renderable.destSize);
  }

  auto getCorner(EntityRenderable const &renderable, claws::Vect<2u, float> corner) const noexcept
  {
    return renderable.destPos + rotate((corner - claws::Vect<2u, float>{0.5f, 0.5f}), renderable.rotation) * renderable.getRadius();
  }

  template<class IT>
  void displayRenderables(IT begin, GLuint count, GLuint texture)
  {
    using T = std::remove_const_t<std::remove_reference_t<decltype(*begin)>>;
    Bind<RenderContext> bind(textureContext);
    GLuint bufferSize(count * 4u * 6u);
    std::unique_ptr<float[]> buffer(new float[bufferSize]);

    for (std::size_t i(0u); i != count; ++i)
      {
	auto renderable(*begin);

	for (unsigned int j(0u); j != 6u; ++j)
	  {
	    constexpr std::array<claws::Vect<2u, float>, 6u> const corners
	    {
	      claws::Vect<2u, float>{0.0f, 0.0f},
		claws::Vect<2u, float>{1.0f, 0.0f},
		  claws::Vect<2u, float>{0.0f, 1.0f},
		    claws::Vect<2u, float>{0.0f, 1.0f},
		      claws::Vect<2u, float>{1.0f, 0.0f},
			claws::Vect<2u, float>{1.0f, 1.0f}
	    };
	    claws::Vect<2u, float> const corner(corners[j]);
	    claws::Vect<2u, float> const sourceCorner(renderable.sourcePos + corner * renderable.sourceSize);
	    claws::Vect<2u, float> const destCorner(getCorner(renderable, corner));

	    std::copy(&sourceCorner[0u], &sourceCorner[2u], &buffer[(j + i * 6u) * 4u]);
	    std::copy(&destCorner[0u], &destCorner[2u], &buffer[(j + i * 6u) * 4u + 2u]);
	  }
	++begin;
      }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindBuffer(GL_ARRAY_BUFFER, textureBuffer);
    my_opengl::setUniform(dim, "dim", textureContext.program);
    my_opengl::setUniform(0u, "tex", textureContext.program);
    glBufferData(GL_ARRAY_BUFFER, bufferSize * sizeof(float), buffer.get(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6 * count);
  }
};
