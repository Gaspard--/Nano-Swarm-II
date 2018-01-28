#pragma once

#include "claws/Vect.hpp"

struct EntityRenderable
{
  // TEXTURE
  /// bottom left corner of source rect
  claws::Vect<2u, float> sourcePos{0.0f, 0.0f};
  /// size of source rect
  claws::Vect<2u, float> sourceSize{0.0f, 0.0f};


  // Destination pos and rotation
  claws::Vect<2u, float> destPos;

  float radius;

  claws::Vect<2u, float> rotation;
};
