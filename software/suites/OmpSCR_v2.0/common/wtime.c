/************************************************************************
  This library is part of the
    OpenMP Source Code Repository

    http://www.pcg.ull.es/OmpSCR/
    e-mail: ompscr@zion.deioc.ull.es

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

**************************************************************************/
/*
* Standard time function
*
* Copyright (C) 2004, Francisco de Sande
*
*/
#include "wtime.h"

#ifdef __ECOS__
#include <time.h>
#include <cyg/kernel/kapi.h>
#else
#include <sys/time.h>
#endif

double OSCR_wtime() {
#ifdef __ECOS__
    static double sec = 0;
    sec = ((double)cyg_current_time() / 100) - sec;
    return sec;
#else
    static int sec = -1;
    struct timeval tv;
    gettimeofday(&tv, (void *)0);
    if (sec < 0) sec = tv.tv_sec;
    return (tv.tv_sec - sec) + 1.0e-6*tv.tv_usec;
#endif
}


