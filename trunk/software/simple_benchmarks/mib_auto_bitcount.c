/* +++Date last modified: 05-Jul-1997 */

/*
**  SNIPTYPE.H - Include file for SNIPPETS data types and commonly used macros
*/

#ifndef SNIPTYPE__H
#define SNIPTYPE__H

#include <stdlib.h>                             /* For free()           */
#include <string.h>                             /* For NULL & strlen()  */

typedef enum {Error_ = -1, Success_, False_ = 0, True_} Boolean_T;

/*#if !defined(WIN32) && !defined(_WIN32) && !defined(__NT__) \
      && !defined(_WINDOWS)
      #if !defined(OS2)*/
  typedef unsigned char  BYTE;
  typedef unsigned long  DWORD;
/* #endif*/
 typedef unsigned short WORD;
/*#else
 #define WIN32_LEAN_AND_MEAN
 #define NOGDI
 #define NOSERVICE
 #undef INC_OLE1
 #undef INC_OLE2
 #include <windows.h>
 #define HUGE
 #endif*/

#define NUL '\0'
#define LAST_CHAR(s) (((char *)s)[strlen(s) - 1])
#define TOBOOL(x) (!(!(x)))
#define FREE(p) (free(p),(p)=NULL)

#endif /* SNIPTYPE__H */
/* +++Date last modified: 05-Jul-1997 */

/*====================================================================

    _MSC_VER        Microsoft C 6.0 and later
    _QC             Microsoft Quick C 2.51 and later
    __TURBOC__      Borland Turbo C, Turbo C++ and BC++
    __BORLANDC__    Borland C++
    __ZTC__         Zortech C and C++
    __SC__          Symantec C++
    __WATCOMC__     WATCOM C
    __POWERC        Mix Power C
    __GNUC__        Gnu C

    Revised:

    25-Sep-95  Bob Stout      Original from PC-PORT.H
    30-Mar-96  Ed Blackman  OS/2 mods for OS/2 ver 2.0 and up
    30-May-96  Andrew Clarke  Added support for WATCOM C/C++ __NT__ macro.
    17-Jun-96  Bob Stout      Added __FLAT__ macros support
    20-Aug-96  Bob Stout      Eliminate Win32 conflicts
======================================================================*/


/* prevent multiple inclusions of this header file */

#ifndef EXTKWORD__H
#define EXTKWORD__H

#include <limits.h>                       /* For INT_MAX, LONG_MAX      */

/*
**  Watcom defines __FLAT__ for 32-bit environments and so will we
*/

#if !defined(__FLAT__) && !defined(__WATCOMC__) && !defined(_MSC_VER)
 #if defined(__GNUC__)
  #define __FLAT__ 1
 #elif defined (_WIN32) || defined(WIN32) || defined(__NT__)
  #define __FLAT__ 1
 #elif defined(__INTSIZE)
  #if (4 == __INTSIZE)
   #define __FLAT__ 1
  #endif
 #elif (defined(__ZTC__) && !defined(__SC__)) || defined(__TURBOC__)
  #if ((INT_MAX != SHRT_MAX) && (SHRT_MAX == 32767))
   #define __FLAT__ 1
  #endif
 #endif
#endif

/*
**  Correct extended keywords syntax
*/

#if defined(__OS2__)        /* EBB: not sure this works for OS/2 1.x */
 #include <os2def.h>
 #define INTERRUPT
 #define HUGE
#elif defined(_WIN32) || defined(WIN32) || defined(__NT__)
 #define WIN32_LEAN_AND_MEAN
 #define NOGDI
 #define NOSERVICE
 #undef INC_OLE1
 #undef INC_OLE2
 #include <windows.h>
 #define INTERRUPT
 #define HUGE
#else /* ! Win 32 or OS/2 */
/* #if (defined(__POWERC) || (defined(__TURBOC__) && !defined(__BORLANDC__)) \
   || (defined(__ZTC__) && !defined(__SC__))) && !defined(__FLAT__)
  #define FAR far
  #define NEAR near
  #define PASCAL pascal
  #define CDECL cdecl
  #if (defined(__ZTC__) && !defined(__SC__)) || (defined(__SC__) && \
        (__SC__ < 0x700))
   #define HUGE far
   #define INTERRUPT
  #else
   #define HUGE huge
   #define INTERRUPT interrupt
   #endif*/
/* #else
   #if (defined(__MSDOS__) || defined(MSDOS)) && !defined(__FLAT__)
   #define FAR _far
   #define NEAR _near
   #define HUGE _huge
   #define PASCAL _pascal
   #define CDECL _cdecl
   #define INTERRUPT _interrupt
   #else*/
   #define FAR
   #define NEAR
   #define HUGE
   #define PASCAL
   #define CDECL
  #endif
/* #endif
   #endif*/

#endif /* EXTKWORD__H */
/* +++Date last modified: 05-Jul-1997 */

/*
**  UNXCONIO.H - Port crucial DOS|Win|OS/2 non-blocking console I/O
**               functions to Unix/Posix.
**
**  public domain SNIPPETS header for use with Steve Poole's TERM_OPT.C
*/

#ifndef UNXCONIO__H
#define UNXCONIO__H

#include <stdio.h>
#include <unistd.h>

#define echo_on()  term_option(0)
#define echo_off() term_option(1)

int term_option();
int getch();

#endif /* UNXCONIO__H */
/* +++Date last modified: 05-Jul-1997 */

/*
**  Macros and prototypes for bit operations
**
**  public domain for SNIPPETS by:
**    Scott Dudley
**    Auke Reitsma
**    Ratko Tomic
**    Aare Tali
**    J. Blauth
**    Bruce Wedding
**    Bob Stout
*/

#ifndef BITOPS__H
#define BITOPS__H

#include <stdio.h>
#include <stdlib.h>                             /* For size_t           */
#include <limits.h>                             /* For CHAR_BIT         */

/*
**  Macros to manipulate bits in any integral data type.
*/

#define BitSet(arg,posn) ((arg) | (1L << (posn)))
#define BitClr(arg,posn) ((arg) & ~(1L << (posn)))
#define BitFlp(arg,posn) ((arg) ^ (1L << (posn)))
#define BitTst(arg,posn) TOBOOL((arg) & (1L << (posn)))

/*
**  Macros to manipulate bits in an array of char.
**  These macros assume CHAR_BIT is one of either 8, 16, or 32.
*/

#define MASK  CHAR_BIT-1
#define SHIFT ((CHAR_BIT==8)?3:(CHAR_BIT==16)?4:8)

#define BitOff(a,x)  ((void)((a)[(x)>>SHIFT] &= ~(1 << ((x)&MASK))))
#define BitOn(a,x)   ((void)((a)[(x)>>SHIFT] |=  (1 << ((x)&MASK))))
#define BitFlip(a,x) ((void)((a)[(x)>>SHIFT] ^=  (1 << ((x)&MASK))))
#define IsBit(a,x)   ((a)[(x)>>SHIFT]        &   (1 << ((x)&MASK)))

/*
**  BITARRAY.C
*/

char *alloc_bit_array(size_t bits);
int   getbit(char *set, int number);
void  setbit(char *set, int number, int value);
void  flipbit(char *set, int number);

/*
**  BITFILES.C
*/

typedef struct  {
      FILE *  file;       /* for stream I/O   */
      char    rbuf;       /* read bit buffer  */
      char    rcnt;       /* read bit count   */
      char    wbuf;       /* write bit buffer */
      char    wcnt;       /* write bit count  */
} bfile;

bfile * bfopen(char *name, char *mode);
int     bfread(bfile *bf);
void    bfwrite(int bit, bfile *bf);
void    bfclose(bfile *bf);

/*
** BITSTRNG.C
*/

void bitstring(char *str, long byze, int biz, int strwid);

/*
**  BSTR_I.C
*/

unsigned int bstr_i(char *cptr);

/*
**  BITCNT_1.C
*/

int CDECL bit_count(long x);

/*
**  BITCNT_2.C
*/

int CDECL bitcount(long i);

/*
**  BITCNT_3.C
*/

int CDECL ntbl_bitcount(long int x);
int CDECL BW_btbl_bitcount(long int x);
int CDECL AR_btbl_bitcount(long int x);

/*
**  BITCNT_4.C
*/

int CDECL ntbl_bitcnt(long x);
int CDECL btbl_bitcnt(long x);

#endif /*  BITOPS__H */
/* +++Date last modified: 05-Jul-1997 */

/*
**  Make an ascii binary string into an integer.
**
**  Public domain by Bob Stout
*/

#include <string.h>

unsigned int bstr_i(char *cptr)
{
      unsigned int i, j = 0;

      while (cptr && *cptr && strchr("01", *cptr))
      {
            i = *cptr++ - '0';
            j <<= 1;
            j |= (i & 0x01);
      }
      return(j);
}


void bitstring(char *str, long byze, int biz, int strwid)
{
      int i, j;

      j = strwid - (biz + (biz >> 2)- (biz % 4 ? 0 : 1));
      for (i = 0; i < j; i++)
            *str++ = ' ';
      while (--biz >= 0)
      {
            *str++ = ((byze >> biz) & 1) + '0';
            if (!(biz % 4) && biz)
                  *str++ = ' ';
      }
      *str = '\0';
}
/* +++Date last modified: 05-Jul-1997 */

/*
**  BITFILES.C - reading/writing bit files
**
**  Public domain by Aare Tali
*/

#include <stdlib.h>

bfile *bfopen(char *name, char *mode)
{
      bfile * bf;

      bf = malloc(sizeof(bfile));
      if (NULL == bf)
            return NULL;
      bf->file = fopen(name, mode);
      if (NULL == bf->file)
      {
            free(bf);
            return NULL;
      }
      bf->rcnt = 0;
      bf->wcnt = 0;
      return bf;
}

int bfread(bfile *bf)
{
      if (0 == bf->rcnt)          /* read new byte */
      {
            bf->rbuf = (char)fgetc(bf->file);
            bf->rcnt = 8;
      }
      bf->rcnt--;
      return (bf->rbuf & (1 << bf->rcnt)) != 0;
}

void bfwrite(int bit, bfile *bf)
{
      if (8 == bf->wcnt)          /* write full byte */
      {
            fputc(bf->wbuf, bf->file);
            bf->wcnt = 0;
      }
      bf->wcnt++;
      bf->wbuf <<= 1;
      bf->wbuf |= bit & 1;
}

void bfclose(bfile *bf)
{
      fclose(bf->file);
      free(bf);
}

/* +++Date last modified: 05-Jul-1997 */

/*
**  BITCNT_4.C - Recursive bit counting functions using table lookup
**
**  public domain by Bob Stout
*/


static char bits_[256] =
{
      0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,  /* 0   - 15  */
      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 16  - 31  */
      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 32  - 47  */
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 48  - 63  */
      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 64  - 79  */
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 80  - 95  */
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 96  - 111 */
      3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 112 - 127 */
      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 128 - 143 */
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 144 - 159 */
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 160 - 175 */
      3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 176 - 191 */
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 192 - 207 */
      3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 208 - 223 */
      3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 224 - 239 */
      4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8   /* 240 - 255 */
};

/*
**  Count bits in each nybble
**
**  Note: Only the first 16 table entries are used, the rest could be
**        omitted.
*/

int CDECL ntbl_bitcnt(long x)
{
      int cnt = bits_[(int)(x & 0x0000000FL)];

      if (0L != (x >>= 4))
            cnt += ntbl_bitcnt(x);

      return cnt;
}

/*
**  Count bits in each byte
*/

int CDECL btbl_bitcnt(long x)
{
      int cnt = bits_[ ((char *)&x)[0] & 0xFF ];

      if (0L != (x >>= 8))
            cnt += btbl_bitcnt(x);
      return cnt;
}

/* +++Date last modified: 05-Jul-1997 */

/*
**  BITCNT_3.C - Bit counting functions using table lookup
**
**  public domain by Auke Reitsma and Bruce Wedding
*/

/*
**  Bits table
*/

static char bits[256] =
{
      0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,  /* 0   - 15  */
      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 16  - 31  */
      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 32  - 47  */
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 48  - 63  */
      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 64  - 79  */
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 80  - 95  */
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 96  - 111 */
      3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 112 - 127 */
      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 128 - 143 */
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 144 - 159 */
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 160 - 175 */
      3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 176 - 191 */
      2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 192 - 207 */
      3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 208 - 223 */
      3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 224 - 239 */
      4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8   /* 240 - 255 */
};

/*
**  Count bits in each nybble
**
**  Note: Only the first 16 table entries are used, the rest could be
**        omitted.
*/

int CDECL ntbl_bitcount(long int x)
{
      return
            bits[ (int) (x & 0x0000000FUL)       ] +
            bits[ (int)((x & 0x000000F0UL) >> 4) ] +
            bits[ (int)((x & 0x00000F00UL) >> 8) ] +
            bits[ (int)((x & 0x0000F000UL) >> 12)] +
            bits[ (int)((x & 0x000F0000UL) >> 16)] +
            bits[ (int)((x & 0x00F00000UL) >> 20)] +
            bits[ (int)((x & 0x0F000000UL) >> 24)] +
            bits[ (int)((x & 0xF0000000UL) >> 28)];
}

/*
**  Count bits in each byte
**
**  by Bruce Wedding, works best on Watcom & Borland
*/

int CDECL BW_btbl_bitcount(long int x)
{
      union
      {
            unsigned char ch[4];
            long y;
      } U;

      U.y = x;

      return bits[ U.ch[0] ] + bits[ U.ch[1] ] +
             bits[ U.ch[3] ] + bits[ U.ch[2] ];
}

/*
**  Count bits in each byte
**
**  by Auke Reitsma, works best on Microsoft, Symantec, and others
*/

int CDECL AR_btbl_bitcount(long int x)
{
      unsigned char * Ptr = (unsigned char *) &x ;
      int Accu ;

      Accu  = bits[ *Ptr++ ];
      Accu += bits[ *Ptr++ ];
      Accu += bits[ *Ptr++ ];
      Accu += bits[ *Ptr ];
      return Accu;
}

int CDECL bitcount(long i)
{
      i = ((i & 0xAAAAAAAAL) >>  1) + (i & 0x55555555L);
      i = ((i & 0xCCCCCCCCL) >>  2) + (i & 0x33333333L);
      i = ((i & 0xF0F0F0F0L) >>  4) + (i & 0x0F0F0F0FL);
      i = ((i & 0xFF00FF00L) >>  8) + (i & 0x00FF00FFL);
      i = ((i & 0xFFFF0000L) >> 16) + (i & 0x0000FFFFL);
      return (int)i;
}

/* +++Date last modified: 05-Jul-1997 */

/*
**  Bit counter by Ratko Tomic
*/


int CDECL bit_count(long x)
{
        int n = 0;
/*
** The loop will execute once for each bit of x set, this is in average
** twice as fast as the shift/test method.
*/
        if (x) do
              n++;
        while (0 != (x = x&(x-1))) ;
        return(n);
}

/* +++Date last modified: 05-Jul-1997 */

/*
**  Functions to maintain an arbitrary length array of bits
*/

char *alloc_bit_array(size_t bits)
{
      char *set = calloc((bits + CHAR_BIT - 1) / CHAR_BIT, sizeof(char));

      return set;
}

int getbit(char *set, int number)
{
        set += number / CHAR_BIT;
        return (*set & (1 << (number % CHAR_BIT))) != 0;    /* 0 or 1   */
}

void setbit(char *set, int number, int value)
{
        set += number / CHAR_BIT;
        if (value)
                *set |= 1 << (number % CHAR_BIT);           /* set bit  */
        else    *set &= ~(1 << (number % CHAR_BIT));        /* clear bit*/
}

void flipbit(char *set, int number)
{
        set += number / CHAR_BIT;
        *set ^= 1 << (number % CHAR_BIT);                   /* flip bit */
}

/* +++Date last modified: 05-Jul-1997 */

/*
**  BITCNTS.C - Test program for bit counting functions
**
**  public domain by Bob Stout & Auke Reitsma
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <float.h>

#define FUNCS  7

static int CDECL bit_shifter(long int x);

int main(int argc, char *argv[])
{
  clock_t start, stop;
  double ct, cmin = DBL_MAX, cmax = 0;
  int i, cminix, cmaxix;
  long j, n, seed;
  int iterations;
  static int (* CDECL pBitCntFunc[FUNCS])(long) = {
    bit_count,
    bitcount,
    ntbl_bitcnt,
    ntbl_bitcount,
    /*            btbl_bitcnt, DOESNT WORK*/
    BW_btbl_bitcount,
    AR_btbl_bitcount,
    bit_shifter
  };
  static char *text[FUNCS] = {
    "Optimized 1 bit/loop counter",
    "Ratko's mystery algorithm",
    "Recursive bit count by nybbles",
    "Non-recursive bit count by nybbles",
    /*            "Recursive bit count by bytes",*/
    "Non-recursive bit count by bytes (BW)",
    "Non-recursive bit count by bytes (AR)",
    "Shift and count bits"
  };

  iterations=20000;

  printf("Bit counter algorithm benchmark\n");

  for (i = 0; i < FUNCS; i++) {
    start = clock();

    for (j = n = 0, seed = rand(); j < iterations; j++, seed += 13)
     n += pBitCntFunc[i](seed);

    stop = clock();
    ct = (stop - start) / (double)1.0e+6;
    if (ct < cmin) {
     cmin = ct;
     cminix = i;
    }
    if (ct > cmax) {
     cmax = ct;
     cmaxix = i;
    }

    printf("%-38s> Time: %7.3f sec.; Bits: %ld\n", text[i], ct, n);
  }
  printf("\nBest  > %s\n", text[cminix]);
  printf("Worst > %s\n", text[cmaxix]);
  return 0;
}

static int CDECL bit_shifter(long int x)
{
  int i, n;

  for (i = n = 0; x && (i < (sizeof(long) * CHAR_BIT)); ++i, x >>= 1)
    n += (int)(x & 1L);
  return n;
}

