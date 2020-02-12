#include "clockTimer.h"
#include "clockAlarm.h"
#include "clockSettings.h"

ClockSettings::ClockSettings( void ) : clockAlarmPtr(std::make_unique<ClockAlarm>()),
				       clockTimerPtr(std::make_unique<ClockTimer>()),
				       _running(true),
				       _local(true),
				       _utcLocalHourDifference(0)
{
  std::chrono::time_point<std::chrono::system_clock> firstTime = std::chrono::system_clock::now();
  _secondsSinceEpoch = std::chrono::system_clock::to_time_t(firstTime);

  struct tm *info;

  time( &_secondsSinceEpoch );
  info = localtime( &_secondsSinceEpoch);
  short localDay = info->tm_mday;
  short localHour = info->tm_hour;

  info = gmtime( &_secondsSinceEpoch);
  short utcDay = info->tm_mday;
  short utcHour = info->tm_hour;

  int dayDifference = localDay - utcDay;

  if (dayDifference == 0)
    {
      _utcLocalHourDifference = localHour - utcHour;
    }
  else if (dayDifference == -1)
    {
      _utcLocalHourDifference = - ((24 - localHour) + utcHour);
    }
  else 
    {
      _utcLocalHourDifference = (24 - utcHour) + localHour;
    } 
}

ClockSettings::~ClockSettings( void )
{
  clockAlarmPtr.reset();
}

bool ClockSettings::running( void )
{
  return _running;
}

void ClockSettings::stopRunning( void )
{
  _running = false;
}
