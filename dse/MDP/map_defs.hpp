#ifndef MAP_DEFS_HPP
#define MAP_DEFS_HPP

#include <map>
#include <list>
#include <string>

#ifdef __GNUC__
#ifdef __GNUC_MINOR__
#if (__GNUC__ >= 4 && __GNUC_MINOR__ >= 3)
#include <tr1/unordered_map>
#define template_map std::tr1::unordered_map
#define hash_fun_char std::tr1::hash<const char *>()
#else
#include <ext/hash_map>
#define  template_map __gnu_cxx::hash_map
#define  hash_fun_char __gnu_cxx::hash<const char *>()
#endif
#else
#include <ext/hash_map>
#define  template_map __gnu_cxx::hash_map
#define  hash_fun_char __gnu_cxx::hash<const char *>()
#endif
#else
#ifdef _WIN32
#include <hash_map>
#define  template_map stdext::hash_map
#define  hash_fun_char stdext::hash_value
#else
#include <map>
#define  template_map std::map
#endif
#endif

#endif
