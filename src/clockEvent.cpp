#include "clockEvent.h"

ClockEvent::ClockEvent( void )
{
}


ClockEvent::ClockEvent(const unsigned int & eventTime) : _time(eventTime)
{
}

ClockEvent::~ClockEvent( void )
{
}

void ClockEvent::setActive( time_t & eventTime )
{
  _time = eventTime;
  _set = true;
}

void ClockEvent::setInActive( void )
{
  _set = false;
}

bool ClockEvent::eventActive( void )
{
  return _set;
}

bool ClockEvent::eventRinging( void )
{
  return _ringing;
}

void ClockEvent::acknowledge( void )
{
  _ringing = false;
}

