#pragma once

#include "NanoBot.hpp"

template<class CollisionContainer, class Access>
inline bool NanoBot::shooterAction(CollisionContainer &nearBots, Battery &source, Access &access, Logic &logic)
{
  static constexpr double attackRange = 0.05;
  static constexpr unsigned cooldown = 300;

  for (auto &entityIndex : nearBots)
    {
      auto entity(access[entityIndex]);
      if ((fixture.pos - entity->fixture.pos).length2() < attackRange * attackRange && !entity->dead)
	{
	  logic.addLaser(fixture.pos, entity->fixture.pos, 1.0);
	  this->cooldown = cooldown;
	  entity->dead = true;
	  return true;
	}
    }
  return false;
}

template<class CollisionContainer, class Access>
inline bool NanoBot::workerAction(CollisionContainer &nearBots, Battery &source, Access &access, Logic &logic)
{
  static constexpr double collectRange = 0.01;
  static constexpr unsigned cooldown = 180;

  {
    return false;
  }
}

template<class CollisionContainer, class Access>
inline bool NanoBot::bruteAction(CollisionContainer &nearBots, Battery &source, Access &access, Logic &logic)
{
  static constexpr double attackRange = 0.01;
  static constexpr unsigned cooldown = 60;

  for (auto &entityIndex : nearBots)
    {
      auto entity(access[entityIndex]);
      if ((fixture.pos - entity->fixture.pos).length2() < attackRange * attackRange && !entity->dead)
	{
	  this->cooldown = cooldown;
	  entity->dead = true;
	  return true;
	}
    }
  return false;
}

template<class CollisionContainer, class Access>
inline bool NanoBot::bomberAction(CollisionContainer &nearBots, Battery &source, Access &access, Logic &logic)
{
  static constexpr double attackRange = 0.01;
  static constexpr double explosionRange = 0.06;

  for (auto &entityIndex : nearBots)
    {
      auto entity(access[entityIndex]);
      if (std::sqrt((fixture.pos - entity->fixture.pos).length2()) < attackRange && !entity->dead)
	{
	  for (auto &toKill : nearBots)
	    {
	      if (std::sqrt((fixture.pos - entity->fixture.pos).length2()) < explosionRange
		  && !toKill->dead)
		toKill->dead = true;
	    }
	  this->dead = true;
	  return true;
	}
    }
  return false;
}

template<class CollisionContainer, class CollisionContainer2, class Access>
inline void NanoBot::ia(CollisionContainer &nearBots, CollisionContainer2 &nearBatteries, Battery &source, Access &access, Logic &logic)
{
  fixture.target = source.fixture.pos;
  // if energy is available go closer to ennemy
  // shoot if there is energy close-by
  if (type == WORKER
      && (workerAction(nearBatteries, source, access, logic)
	  || workerAction(nearBots, source, access, logic)))
    return;
  else if (type == BRUTE
	   && (bruteAction(nearBatteries, source, access, logic)
	       || bruteAction(nearBots, source, access, logic)))
    return;
  else if (type == SHOOTER
	   && (shooterAction(nearBatteries, source, access, logic)
	       || shooterAction(nearBots, source, access, logic)))
    return;
  else if (type == BOMBER
	   && (bomberAction(nearBatteries, source, access, logic)
	       || bomberAction(nearBots, source, access, logic)))
    return;
}
