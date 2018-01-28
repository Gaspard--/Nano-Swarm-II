#include "Battery.hpp"
#include "EntityManager.hpp"

Battery::Battery(int power)
  : power(power)
{
}

void Battery::ia(EntityManager& em)
{
  // recharge
  // go to nanobots
  // repeat
}

void Battery::update()
{

}

void Battery::setPower(int power)
{
  this->power = power;
}

int Battery::getPower() const
{
  return power;
}
