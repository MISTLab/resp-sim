#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>

#ifndef STATIC_PLATFORM
#include <exception>
#include <stdexcept>
#include <execinfo.h>
#include <signal.h>
#endif

#ifdef STATIC_PLATFORM
namespace resp{
void killAll(std::string errorMsg);
}
#endif

#ifdef MAKE_STRING
#undef MAKE_STRING
#endif
#define MAKE_STRING( msg )  ( ((std::ostringstream&)((std::ostringstream() << '\x0') << msg)).str().substr(1) )

#ifdef THROW_EXCEPTION
#undef THROW_EXCEPTION
#endif
#ifdef STATIC_PLATFORM
#define THROW_EXCEPTION( msg ) ( resp::killAll(MAKE_STRING( "At: function " << __PRETTY_FUNCTION__ << " file: " << __FILE__ << ":" << __LINE__ << " --> " << msg )) )
#else
namespace resp{
void RaiseTraceException(std::string message);
}
#define THROW_EXCEPTION( msg ) ( resp::RaiseTraceException(MAKE_STRING( "At: function " << __PRETTY_FUNCTION__ << " file: " << __FILE__ << ":" << __LINE__ << " --> " << msg )) )
#endif

#endif
