#ifndef _CLOCK_TIMER_H_
#define _CLOCK_TIMER_H_

#include "clockEvent.h"

class ClockTimer: public ClockEvent
{
 public:
  ClockTimer( void );
  ClockTimer(const unsigned int & eventTime);
  ~ClockTimer( void );
  void setActive( time_t & timeEvent );
  void setInActive( void );
  bool eventActive( void );
  bool eventRinging( void );
  void acknowledge( void );
  time_t decrementAndReturnActiveTimer( void );
 private:
  time_t _time;
  bool _set;
  bool _ringing;  
};
#endif
