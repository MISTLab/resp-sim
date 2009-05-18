#include <map>
#include <list>
#include <string>

#include <boost/pool/pool_alloc.hpp>
#include <ext/malloc_allocator.h>

class PluginIf;

#ifndef MAP_DEFS
#define MAP_DEFS
/// Type definitions for differen kinds of maps
// typedef std::list<std::pair<float, float>, __gnu_cxx::malloc_allocator<std::pair<float, float> > > list_pair_float_float;
//
// typedef std::map<std::string, int, std::less<std::string>, __gnu_cxx::malloc_allocator< std::pair<std::string const, int> > > int_map;
// typedef std::map<std::string, float, std::less<std::string>, __gnu_cxx::malloc_allocator< std::pair<std::string const, float> > > float_map;
// typedef std::map<std::string, double, std::less<std::string>, __gnu_cxx::malloc_allocator< std::pair<std::string const, double> > > double_map;
// typedef std::map<std::string, std::pair<float, float>, std::less<std::string>, __gnu_cxx::malloc_allocator< std::pair<std::string const, std::pair<float,float> > > > metric_map;
// typedef std::multimap<std::string, int, std::less<std::string>, __gnu_cxx::malloc_allocator< std::pair<std::string const, int> > > int_multimap;
// typedef std::map<std::string, list_pair_float_float,
//                  std::less<std::string>,
//                  __gnu_cxx::malloc_allocator< std::pair<std::string const,
//                  list_pair_float_float> > > list_float_map;

// typedef std::list<std::pair<float, float>, boost::fast_pool_allocator<std::pair<float, float> > > list_pair_float_float;
//
// typedef std::map<std::string, int, std::less<std::string>, boost::pool_allocator< std::pair<std::string const, int> > > int_map;
// typedef std::map<std::string, float, std::less<std::string>, boost::pool_allocator< std::pair<std::string const, float> > > float_map;
// typedef std::map<std::string, double, std::less<std::string>, boost::pool_allocator< std::pair<std::string const, double> > > double_map;
// typedef std::map<std::string, std::pair<float, float>, std::less<std::string>, boost::pool_allocator< std::pair<std::string const, std::pair<float,float> > > > metric_map;
// typedef std::multimap<std::string, int, std::less<std::string>, boost::pool_allocator< std::pair<std::string const, int> > > int_multimap;
// typedef std::map<std::string, list_pair_float_float,
//                  std::less<std::string>,
//                  boost::pool_allocator< std::pair<std::string const,
//                  list_pair_float_float> > > list_float_map;


typedef std::list<std::pair<float, float> > list_pair_float_float;
typedef std::map<std::string, int> int_map;
typedef std::map<int, std::string> reverse_int_map;
typedef std::map<std::string, float> float_map;
typedef std::map<std::string, double> double_map;
typedef std::map<int, float> stats_map;
typedef std::map<std::string, std::pair<float, float> > metric_map;
typedef std::multimap<std::string, int> int_multimap;
typedef std::map<std::string, list_pair_float_float> list_float_map;

typedef std::map<PluginIf*, int> plugin_int_map;
typedef std::map<PluginIf*, float> plugin_float_map;
typedef std::map<PluginIf*, double> plugin_double_map;
typedef std::multimap<PluginIf*, int> plugin_int_multimap;
typedef std::map<PluginIf*, list_pair_float_float> plugin_list_float_map;

#endif
