#include "Battery.hpp"
#include "EntityManager.hpp"

Battery::Battery(unsigned int power)
  : power(power)
{
}

void Battery::ia(EntityManager& em)
{
  // recharge
  // go to nanobots
  // repeat
}

void Battery::reload(Battery &source)
{
  constexpr unsigned int step(16u);

  unsigned int take(std::min(std::min(source.power, step), maxPower -power));

  source.power -= take;
  power += take;
}

void Battery::update()
{

}

void Battery::setPower(unsigned int power)
{
  this->power = power;
}

int Battery::getPower() const
{
  return power;
}
