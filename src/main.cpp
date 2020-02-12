#include <getopt.h>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>              // std::thread
#include <future>              // std::promise, std::future
#include <iostream>            // std::cout
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cstdlib>
#include <sys/time.h>
#include <sys/types.h>
#include <algorithm>
#include <cctype>

#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>



#include "dbgMsg.h"
#include "clockEvent.h"
#include "clockTimer.h"
#include "clockSettings.h"

std::mutex ClockSettingsLock;
bool ClockTickToProcess = false;

static const unsigned int charsInClockLine = 60;

static bool stop = false;

void intHandler(int dummy)
{
    stop = true;
}

template <class T>
bool inRange(T value, T lower, T upper)
{
  return ((lower <= value) && (value <= upper));
}


void printHelp(const char * programName)
{
    std::cout <<
      "Usage: " << programName <<
      "\n\t[--alarm <HH:MM>]        Set Alarm to Hour[0-23]:Minute[0-59]\n"
      "\t[--timer <seconds>]      Set Timer to Seconds from now\n"
      "\t[--utc]                  Set UTC (GMT) instead of local time\n"
      "\t[--help]                 Show this message and exit\n"
      "\t[--level]                Set debug output level (0:none, 1:verbose, 2:default)\n"
      "\n\nDefaults are no alarm set, no timer set, local offset from UTC, and appropriate DST\n\n"
      "Example:" << programName << " --alarm 21:12 --timer 600  starts the clock with an alarm time of 9:21 PM and timer expiring in 10 minutes\n\n";
    return;
}

void setUTC(ClockSettings & theClockSettings)
{
   theClockSettings._local = false;
}

void setDebugLevel(const char * optarg, ClockSettings & theClockSettings)
{
  int debugLevel = std::stoi(optarg);
  switch(debugLevel)
    {
    case 0:
      DbgSetTo = DbgLvl::DBGMSG_NONE;
      break;
    case 1:
      DbgSetTo = DbgLvl::DBGMSG_VERBOSE;
      break;
    case 2:
      DbgSetTo = DbgLvl::DBGMSG_DEFAULT;
      break;
      // no default case, no action if not valid level
    };
}

bool validateTimerArgument(const char * timer, ClockSettings & theClockSettings)
{
  bool retValue = true;
  
  std::istringstream iss(timer);
  time_t convertTimer;
  iss >> convertTimer;
  if (!iss.fail())
    {
      theClockSettings.clockTimerPtr->setActive(convertTimer);
    }
  else
    {
      std::cout << " ERROR: validateTimerArgument argument " << convertTimer << " fails validation.\n";
      retValue = false;
    }
  return retValue;
}

bool validateAlarmArgument(const char * alarm, ClockSettings & theClockSettings)
{
  bool retValue = false;
  std::string alarmStr(alarm);
  std::vector<std::string> alarmInParts;
  std::stringstream s_stream(alarmStr);

  while (s_stream.good())
    {
      std::string alarmPart;
      getline(s_stream, alarmPart,':');
      alarmInParts.emplace_back(alarmPart);
    }
  if (alarmInParts.size() == 2)
    {
      int hourNumber;
      std::istringstream hour (alarmInParts.at(0));
      hour >> hourNumber;
      if (!hour.fail() && (inRange(hourNumber,0,23))) // ((hourNumber >= 0) && (hourNumber <= 23)))
        {
          int minuteNumber;
          std::istringstream minute (alarmInParts.at(1));
          minute >> minuteNumber;
          if (!minute.fail() && ((minuteNumber >= 0) && (minuteNumber <= 59)))
            {
              retValue = true;
	      theClockSettings.clockAlarmPtr->setClockAlarmTimeSecondsInDay((hourNumber*3600) + (minuteNumber * 60));
            }
        }
    }
  if (retValue == false)
    {
      std::cout << " ERROR: Invalid argument " << alarm << "\n";
    }
  return retValue;
}

bool ProcessCommandLineArgs(int argc, char** argv, ClockSettings & theClockSettings)
{
    const char* const short_opts = "a:t:l:hu";
    const option long_opts[] = {
            {"alarm", required_argument, nullptr, 'a'},
            {"timer", required_argument, nullptr, 't'},
            {"level", required_argument, nullptr, 'l'},
            {"utc", no_argument, nullptr, 'u'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, no_argument, nullptr, 0}
    };

    bool validCommandLineArgs = true;
    bool justHelpRequest = false;

    while (validCommandLineArgs)
    {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

        if (-1 == opt)
            break;

        switch (opt)
          {
          case 'a':
            validCommandLineArgs = validateAlarmArgument(optarg,theClockSettings);
            break;
          case 't':
            validCommandLineArgs = validateTimerArgument(optarg,theClockSettings);
            break;
          case 'l':
            setDebugLevel(optarg, theClockSettings);
          break;
          case 'u':
            setUTC(theClockSettings);
            break;
          case 'h': // -h or --help
            // fall through
          case '?': // Unrecognized option, falls though to default
          default:
            validCommandLineArgs = false;
            break;
          }
    }
    if (!validCommandLineArgs)
      {
        printHelp(argv[0]);
      }
    return validCommandLineArgs;
}

void clockPulseGenerator(ClockSettings & tClockSettings,std::promise<std::time_t>& prom) 
{
  bool controlHysteresis = false;
  std::chrono::system_clock::time_point tPoint = std::chrono::system_clock::now();
  std::time_t startTime = std::chrono::system_clock::to_time_t(tPoint);
  unsigned int sleepInMilliSeconds = 1000;

  // For debugging unsigned int count = 0;
	  
  // For debugging while (tClockSettings.running() && count++ < 8)
  while (tClockSettings.running())
    {
      std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
      auto startDuration = start.time_since_epoch();
      
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepInMilliSeconds));
      
      std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
      auto duration = end.time_since_epoch();
      
      auto millis =
	std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() -
	std::chrono::duration_cast<std::chrono::milliseconds>(startDuration).count();
      
      if ((!controlHysteresis) && ((sleepInMilliSeconds = 1000 + (1000 - millis)) != 1000))
	{
	  controlHysteresis = true;;
	}
      else
	{
	  sleepInMilliSeconds = 1000;
	  controlHysteresis = false;
	}
      std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
      try
	{
	  //using a local lock guard to lock mutex guarantees unlocking on destruction / exception
	  std::lock_guard<std::mutex> lck(ClockSettingsLock);
	  tClockSettings._secondsSinceEpoch = std::chrono::system_clock::to_time_t(now);
	        
	  // DbgMsg<<DbgMsgVerbose<<"Producing " << sleepInMilliSeconds << " end " << std::put_time(std::localtime(&tClockSettings._secondsSinceEpoch), "%F %T")   <<std::endl;
	  ClockTickToProcess = true;

	}
      catch (std::logic_error&)
	{
	}
      // mutex unlocking (via try scope) is done before notifying, to avoid waking up
      // the waiting thread only to block again (see notify_one for details)
      tClockSettings._condVar.notify_one(); 

    }
  prom.set_value(tClockSettings._secondsSinceEpoch - startTime);

  tClockSettings.stopRunning();
  
  // DbgMsg<<DbgMsgVerbose<<"Exiting Producing... \n" ;
}

void finishClockLineOutput(const int & spaces)
{
  for (int i = 0; i < spaces; i++)
    {
      std::cout << ' ';
    }
  std::cout << "||\n";
}


void clockPulseResponse(ClockSettings & tClockSettings)
{
  
  while (tClockSettings.running())
    {
      std::tm tm;
      try
	{
	     std::unique_lock<std::mutex> lk(ClockSettingsLock);
	     tClockSettings._condVar.wait(lk, []{return ClockTickToProcess;});
	     
             if (tClockSettings._local == true)
	       {
		 std::tm tm = *std::localtime(&tClockSettings._secondsSinceEpoch);
	       }
	     else
	       {
		 std::tm tm = *std::gmtime(&tClockSettings._secondsSinceEpoch);
	       }
	     std::stringstream ss;
	     ss << std::put_time(&tm, "%F %T");
	     time_t timerT = tClockSettings.clockTimerPtr->decrementAndReturnActiveTimer();

	     if (tClockSettings._local)
	       {
		 tClockSettings.clockAlarmPtr->checkAlarm(tClockSettings._secondsSinceEpoch + (tClockSettings._utcLocalHourDifference * 3600));
	       }
	     else
	       {
		 tClockSettings.clockAlarmPtr->checkAlarm(tClockSettings._secondsSinceEpoch);
	       }
	     std::cout << "\033c" <<"\n"; // clear screen
	     char * displayName = tClockSettings.manufacturer.clockName();
	     if (displayName != nullptr)
	       {
		 std::size_t n = std::strlen(displayName);
		 std::string manName = "|| Manufacturer: " + std::string(displayName);
		 std::cout << manName;
		 int spaces = charsInClockLine - manName.length() - 3;
		 finishClockLineOutput(spaces);
	       }
	     std::cout << "||-------------------------------------------------------||\n";
	     std::cout << "||-------------------------------------------------------||\n";
	     std::string tmp = "|| Time: " + (ss.str());
	     int spaces = charsInClockLine - tmp.length() - 3;
	     std::cout << tmp;
	     finishClockLineOutput(spaces);
	     
	     if (tClockSettings.clockAlarmPtr->eventRinging())
	       {
		 std::cout << "|| ALARM RINGING                                         ||\n";
	       }
	     else if (tClockSettings.clockAlarmPtr->eventActive())
	       {
		 std::stringstream ss;
		 ss << "|| ALARM SET " <<std::setw(2) << std::setfill('0') << std::to_string(tClockSettings.clockAlarmPtr->_alarmHour) << ":"
		    << std::setw(2) << std::setfill('0') << std::to_string(tClockSettings.clockAlarmPtr->_alarmMinute);
		 // tmp = "|| ALARM SET " + std::to_string(tClockSettings.clockAlarmPtr->_alarmHour) + ":" + std::to_string(tClockSettings.clockAlarmPtr->_alarmMinute);
		 tmp = ss.str();
		 std::cout << tmp;
		 spaces = charsInClockLine -tmp.length() - 3;
		 finishClockLineOutput(spaces);
	       }
	     else
	       {
		 std::cout << "|| Alarm: Not Set                                        ||\n";
	       }
	     if (tClockSettings.clockTimerPtr->eventRinging())
	       {
		 std::cout << "|| Timer: RINGING                                        ||\n";
	       }
	     else if (tClockSettings.clockTimerPtr->eventActive())
	       {
		 tmp = "|| Timer: ACTIVE " + std::to_string(timerT) + " Seconds left ...";
		 std::cout << tmp;
		 spaces = charsInClockLine -tmp.length() - 3;
		 finishClockLineOutput(spaces);
	       }
	     else
	       {
		 std::cout << "|| Timer: Not Running                                    ||\n";
	       }
	     std::cout << "||-------------------------------------------------------||\n";
	     std::cout << "||-------------------------------------------------------||\n";
	     
	       // DbgMsg<<DbgMsgVerbose<<"Response " << std::put_time(std::localtime(&tClockSettings._secondsSinceEpoch), "%F %T")   <<std::endl;
	     ClockTickToProcess = false;
	     
	}
      catch (std::logic_error&)
        {
        }

    }
  // do stuff...
}

void clockUserInput(ClockSettings & tClockSettings)
{
    struct pollfd pfd = { STDIN_FILENO, POLLIN, 0 };

    std::string userInput;
    int ret = 0;
    
    while(!stop && tClockSettings.running())
      {
	ret = poll(&pfd, 1, 1000);  // timeout of 1000ms
	if (ret == 1) // there is something to read
	  {
	    std::getline(std::cin, userInput);
	    try
	      {
		std::unique_lock<std::mutex> lk(ClockSettingsLock);
		switch(userInput.at(0))
		  {
		  case 'A':
		    userInput = userInput.substr(1);
		    userInput.erase(std::remove_if(userInput.begin(), userInput.end(), ::isspace), userInput.end());
		    validateAlarmArgument(userInput.c_str(), tClockSettings);
		    break;
		  case 'T':
		    userInput = userInput.substr(1);
		    userInput.erase(std::remove_if(userInput.begin(), userInput.end(), ::isspace), userInput.end());
		    validateTimerArgument(userInput.c_str(), tClockSettings);
		    break;
		  case 'O':
		    userInput = userInput.substr(1);
		    userInput.erase(std::remove_if(userInput.begin(), userInput.end(), ::isspace), userInput.end());
		    if (userInput.length() == 1)
		      {
			if (userInput.at(0) == 'A')
			  {
			    tClockSettings.clockAlarmPtr->setInActive();			    
			  }
			else if (userInput.at(0) == 'T')
			  {
			    tClockSettings.clockTimerPtr->setInActive();
			  }
		      }
		  case 'E':
		    if (tClockSettings.clockTimerPtr->eventRinging())
		      {
			tClockSettings.clockTimerPtr->acknowledge();
		      }
		    if (tClockSettings.clockAlarmPtr->eventRinging())
		      {
			tClockSettings.clockAlarmPtr->acknowledge();
		      }
		    break;
		  default:
		    std::cout << " ERROR: Received UNKNOWN COMMAND " << userInput.at(0) << "\n";
		    break;
		  };
	      }
	    catch( ...)
	      {
	      }
	  }
	else if(ret == -1)
	  {
	    std::cout << " ERROR: " << strerror(errno) << std::endl;
	  }
	/*
	else
	  {
            std::cout << "Timed out ..." << std::endl;
	  }
	*/
      }
    tClockSettings.stopRunning();
    std::cout << " INFO: graceful shutdown of reader " << std::endl;
}


int main(int argc, char * argv[]) 
{

  // DbgMsg<<DbgMsgDefault<<"default:default"<<std::endl;
  // DbgMsg<<DbgMsgVerbose<<"verbose:verbose"<<std::endl;
  // DbgMsg<<DbgMsgNone<<"none:default"<<std::endl;

  signal(SIGINT, intHandler);
  signal(SIGKILL, intHandler);

  ClockSettings theClockSettings;
  ClockTimer theClockTimer;

  if (ProcessCommandLineArgs(argc, argv, theClockSettings))
    {
      std::promise<std::time_t> runningTimePromise;
      std::future<std::time_t>  runningTime = runningTimePromise.get_future();

      // spawn new thread that calls clockPulseGenerator(theClockSettings,runningTimePromise)
      std::thread first (clockPulseGenerator,std::ref(theClockSettings),std::ref(runningTimePromise));
      
      // spawn new thread that calls clockPulseResponse(theClockSettings)
      std::thread second (clockPulseResponse,std::ref(theClockSettings));     

      // spawn new thread that calls clockUserInput(theClockSettings)
      std::thread third (clockUserInput,std::ref(theClockSettings));     
      
      // synchronize threads
      first.join();                // pauses until first finishes
      second.join();               // pauses until second finishes
      third.join();

      std::ofstream outfile;
      outfile.open("rubicWriteToFile.txt");
      if (outfile.is_open())
	{
	  outfile << "# of one second ticks produced = " << runningTime.get() << std::endl;
	  outfile << "clockPulseGenerator and clockPulseResponse completed.\n";
	  outfile.close();
	}
      else
	{
	  std::cout << "ERROR: failed to open file rubicWriteToFile.txt for writing.\n";
	}
    }
  else
    {
      std::cout << "ERROR: clockPulseGenerator and clockPulseResponse threads failed to start.\n";
    }

  return 0;

}
