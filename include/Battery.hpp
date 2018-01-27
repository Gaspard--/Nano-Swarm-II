#pragma once

#include "Entity.hpp"

class Battery : public Entity
{
private:
  int power;

public:
  Battery(int power=100);
  ~Battery() = default;
};
