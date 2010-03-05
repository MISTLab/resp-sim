#include <pthread.h>
#include "mutex.h"

void gomp_mutex_init (gomp_mutex_t *mutex)
{
  pthread_mutex_init (mutex, NULL);
}

void gomp_mutex_lock (gomp_mutex_t *mutex)
{
  pthread_mutex_lock (mutex);
}

void gomp_mutex_unlock (gomp_mutex_t *mutex)
{
   pthread_mutex_unlock (mutex);
}

void gomp_mutex_destroy (gomp_mutex_t *mutex)
{
  pthread_mutex_destroy (mutex);
}
