#ifndef _CLOCK_SETTINGS_H_
#define _CLOCK_SETTINGS_H_

#include <memory>
#include <ctime>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include "clockAlarm.h"
#include "ruleOfFiveClockName.h"

class ClockSettings
{
 public:
  ClockSettings( void );
  ~ClockSettings( void );
  void setTimer(unsigned int & timerSetTo);
  bool running();
  void stopRunning();
  
  std::unique_ptr<ClockAlarm> clockAlarmPtr;
  std::unique_ptr<ClockTimer> clockTimerPtr;

  std::time_t _secondsSinceEpoch;
  std::condition_variable _condVar;

  bool _local;
  short _utcLocalHourDifference;
  ruleOfFiveClockName manufacturer;
  
 private:
  std::atomic<bool> _running;

};
#endif
