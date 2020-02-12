#ifndef _CLOCK_EVENT_H_
#define _CLOCK_EVENT_H_

#include <ctime>

class ClockEvent
{
 public:
  ClockEvent( void );
  ClockEvent(const unsigned int & eventTime);
  ~ClockEvent( void );
  virtual void setActive( time_t & timeEvent );
  virtual void setInActive( void );
  virtual bool eventActive( void );
  virtual bool eventRinging( void );
  virtual void acknowledge( void );
 private:
  time_t _time;
  bool _set;
  bool _ringing;  
};

#endif
