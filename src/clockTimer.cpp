#include "clockTimer.h"

ClockTimer::ClockTimer( void ) : _set{false}, _time{0}, _ringing{false}
{
}

ClockTimer::ClockTimer(const unsigned int & eventTime) : _set{true}, _time{eventTime}, _ringing{false}
{
}

ClockTimer::~ClockTimer( void )
{
}

void ClockTimer::setActive( time_t & timeEvent )
{
  _time = timeEvent;
  _set = true;
}

void ClockTimer::setInActive( void )
{
  _set = false;
  _ringing = false;
  _time = 0;
}

bool ClockTimer::eventActive( void )
{
  return _set;
}

bool ClockTimer::eventRinging( void )
{
  return _ringing;
}

void ClockTimer::acknowledge( void )
{
  _set = false;
  _ringing = false;
  _time = 0;
}

time_t ClockTimer::decrementAndReturnActiveTimer( void )
{
  if ((_ringing == false) && (_set == true))
    {
      if ((_time = _time - 1) == 0)
	{
	  _ringing = true;
	}
      
    }
  return _time;
}
