#pragma once

#include <unordered_map>

struct DisplayInfo
{
  std::unordered_map<GLuint, std::vector<Renderable>> renderables;
  std::unordered_map<GLuint, std::vector<EntityRenderable>> entityRenderables;
};
