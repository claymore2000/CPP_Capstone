#ifndef _RULEOFFIVECLOCKNAME_H_
#define _RULEOFFIVECLOCKNAME_H_

#include <cstring>
#include <memory>

class ruleOfFiveClockName
{
 private: 
  char * cstring;
 public:
 ruleOfFiveClockName(const char * s = "Big Bubba") : cstring(nullptr)
    {
        if (s)
	  {
            std::size_t n = std::strlen(s) + 1;
            cstring = new char[n];      // allocate                                                                                                                                       
            std::memcpy(cstring, s, n); // populate                                                                                                                                       
        }

    }
 
    ~ruleOfFiveClockName()
    {
      if (cstring != nullptr)
	{
	  delete[] cstring;  // deallocate
	} 
    }
 
    ruleOfFiveClockName(const ruleOfFiveClockName& other) // copy constructor
    : ruleOfFiveClockName(other.cstring)
    {}
 
    ruleOfFiveClockName(ruleOfFiveClockName&& other) noexcept // move constructor
    : cstring(std::exchange(other.cstring, nullptr))
    {}
 
    ruleOfFiveClockName& operator=(const ruleOfFiveClockName& other) // copy assignment
    {
         return *this = ruleOfFiveClockName(other);
    }
 
    ruleOfFiveClockName& operator=(ruleOfFiveClockName&& other) noexcept // move assignment
    {
        std::swap(cstring, other.cstring);
        return *this;
    }

    char * clockName( void)
    {
      return cstring;
    }
 
// alternatively, replace both assignment operators with 
//  ruleOfFiveClockName& operator=(ruleOfFiveClockName other) noexcept
//  {
//      std::swap(cstring, other.cstring);
//      return *this;
//  }
};

#endif
