#include "Animation.hpp"

Animation::Animation(int nbAnimation)
  : animationFrame(0), internalFrame(0), nbAnimation(nbAnimation)
{
}

void Animation::animate()
{
  if (++internalFrame > timePerFrame)
    {
      internalFrame = 0;
      if (++animationFrame > nbAnimation)
	animationFrame = 0;
    }
}

float Animation::getAnimationFrame() const
{
  return animationFrame;
}
