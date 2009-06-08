#ifndef CPU_TIME_HPP
#define CPU_TIME_HPP

#include <boost/lexical_cast.hpp>

#ifdef WIN32
#include <time.h>
#else
#include <sys/times.h>
#include <unistd.h>
#endif

#if defined(CLK_TCK)
#define TIMES_TICKS_PER_SEC CLK_TCK
#elif defined(_SC_CLK_TCK)
#define TIMES_TICKS_PER_SEC sysconf(_SC_CLK_TCK)
#elif defined(HZ)
#define TIMES_TICKS_PER_SEC HZ
#else // !CLK_TCK && !_SC_CLK_TCK && !HZ
#define TIMES_TICKS_PER_SEC 60
#endif // !CLK_TCK && !_SC_CLK_TCK && !HZ

namespace resp{

/**
 * return a long which represents the elapsed processor
 * time in milliseconds since some constant reference
*/
static inline unsigned long cpu_time()
{
   unsigned long t;
#ifdef WIN32
   t = (clock() * 1000) / TIMES_TICKS_PER_SEC;
#else
   struct tms now;
   clock_t    ret = times(&now);
   if (ret == (clock_t)-1)
      now.tms_utime = now.tms_stime = ret = 0;
   t = (long(now.tms_utime) * 1000) / TIMES_TICKS_PER_SEC;
#endif
   return t;
}

/**
 *  massage a long which represents a time interval in
 *  milliseconds, into a string suitable for output
 */
static inline std::string print_cpu_time(unsigned long t)
{
   std::string ost;
   ost = boost::lexical_cast<std::string>(t / 1000) + ".";
   unsigned long centisec = (t % 1000) / 10;
   if (centisec < 10)
      ost += "0" + boost::lexical_cast<std::string>(centisec);
   else
      ost += boost::lexical_cast<std::string>(centisec);
   return ost;
}

}

#endif
