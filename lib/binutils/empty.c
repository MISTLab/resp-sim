#ifndef HAVE_ENVIRON_DECL
#if (defined(__MACOSX__) || defined(__DARWIN__) || defined(__APPLE__))
#include <crt_externs.h>
char **environ(){
    return (*_NSGetEnviron());
}
#endif
#endif

