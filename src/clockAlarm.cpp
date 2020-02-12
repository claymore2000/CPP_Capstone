#include "clockAlarm.h"

ClockAlarm::ClockAlarm() : _alarmHour(0), _alarmMinute(0), _time(0), _set(false), _ringing(false) 
{
}

ClockAlarm::ClockAlarm(const unsigned int & alarmTime)
{
  
}

ClockAlarm::~ClockAlarm()
{

}

void ClockAlarm::setClockAlarmTimeHour(const unsigned int & alarmTimeHour)
{
  _alarmHour = alarmTimeHour;
}

void ClockAlarm::setClockAlarmTimeMinute(const unsigned int & alarmTimeMinute)
{
  _alarmMinute = alarmTimeMinute;
}

void ClockAlarm::setActive( void )
{
  _set = true;
}

void ClockAlarm::setClockAlarmTimeSecondsInDay(const unsigned int & alarmTimeInSeconds)
{
  _time = alarmTimeInSeconds;
  _alarmHour = alarmTimeInSeconds / 3600;
  _alarmMinute = (alarmTimeInSeconds - (_alarmHour * 3600)) / 60;
  _set = true;
}

void ClockAlarm::setInActive( void )
{
  _set = false;
}

bool ClockAlarm::eventActive( void )
{
  return _set;
}

bool ClockAlarm::eventRinging( void )
{
  return _ringing;
}

void ClockAlarm::acknowledge( void )
{
  _ringing = false;
  _set = false;
}

void ClockAlarm::checkAlarm( const time_t & secondsSinceEpoch )
{
  if (_set == true)
    {
      unsigned int secondsInDay = secondsSinceEpoch % 86400;
      unsigned int alarmHour = secondsInDay / 3600;
      unsigned int alarmMinute = (secondsInDay - (alarmHour * 3600)) / 60;
      if ((alarmHour == _alarmHour) && (alarmMinute == _alarmMinute))
	{
	  _ringing = true;
	}
    }
}
  
