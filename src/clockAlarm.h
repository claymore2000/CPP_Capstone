#ifndef _CLOCK_ALARM_H_
#define _CLOCK_ALARM_H_

#include "clockEvent.h"

class ClockAlarm: public ClockEvent
{
 public:
  ClockAlarm( void );
  ClockAlarm(const unsigned int & eventTime);
  ~ClockAlarm( void );
  void setClockAlarmTimeHour(const unsigned int & alarmTimeHour);
  void setClockAlarmTimeMinute(const unsigned int & alarmTimeMinute);
  void setClockAlarmTimeSecondsInDay(const unsigned int & alarmTimeInSeconds);
  void checkAlarm( const time_t & secondsSinceEpoch );
  void setActive( void );
  void setInActive( void );
  bool eventActive( void );
  bool eventRinging( void );
  void acknowledge( void );
  unsigned short _alarmHour;
  unsigned short _alarmMinute;
 private:
  time_t _time;
  bool _set;
  bool _ringing;
  
};
#endif
