#ifndef ANIMATION_HPP_
# define ANIMATION_HPP_

class Animation
{
public:
  Animation(int nbAnimation);
  ~Animation() = default;

  void animate();
  float getAnimationFrame() const;

private:
  int animationFrame;
  int internalFrame;
  int timePerFrame = 120;
  int nbAnimation;
};

#endif /* !ANIMATION_HPP_ */
