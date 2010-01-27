/***************************************************************************
*                            Bit Stream File Header
*
*   File    : bitfile.h
*   Purpose : Provides definitions and prototypes for a simple library of
*             I/O functions for files that contain data in sizes that aren't
*             integral bytes.
*             An attempt was made to make the functions in this library
*             analogous to functions provided to manipulate byte streams.
*             The functions contained in this library were created with
*             compression algorithms in mind, but may be suited to other
*             applications.
*   Author  : Michael Dipperstein
*   Date    : January 9, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: bitfile.h,v 1.6 2007/08/26 21:53:48 michael Exp $
*   $Log: bitfile.h,v $
*   Revision 1.6  2007/08/26 21:53:48  michael
*   Changes required for LGPL v3.
*
*   Revision 1.5  2006/06/03 19:33:11  michael
*   Used spell checker to correct spelling.
*
*   Revision 1.4  2005/12/06 15:06:37  michael
*   Added BitFileGetBitsInt and BitFilePutBitsInt for integer types.
*
*   Revision 1.3  2004/11/09 14:16:58  michael
*   Added functions to convert open bit_file_t to FILE and to
*   align open bit_file_t to the next byte.
*
*   Revision 1.2  2004/06/15 13:16:10  michael
*   Use incomplete type to hide definition of bitfile structure
*
*   Revision 1.1.1.1  2004/02/09 05:31:42  michael
*   Initial release
*
*
****************************************************************************
*
* Bitfile: Bit stream File I/O Routines
* Copyright (C) 2004-2007 by Michael Dipperstein (mdipper@cs.ucsb.edu)
*
* This file is part of the bit file library.
*
* The bit file library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The bit file library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/
typedef enum
{
    BF_READ = 0,
    BF_WRITE = 1,
    BF_APPEND= 2,
    BF_NO_MODE
} BF_MODES;

/* incomplete type to hide implementation */
struct bit_file_t;
typedef struct bit_file_t bit_file_t;

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/* open/close file */
bit_file_t *BitFileOpen(const char *fileName, const BF_MODES mode);
bit_file_t *MakeBitFile(FILE *stream, const BF_MODES mode);
int BitFileClose(bit_file_t *stream);
FILE *BitFileToFILE(bit_file_t *stream);

/* toss spare bits and byte align file */
int BitFileByteAlign(bit_file_t *stream);

/* get/put character */
int BitFileGetChar(bit_file_t *stream);
int BitFilePutChar(const int c, bit_file_t *stream);

/* get/put single bit */
int BitFileGetBit(bit_file_t *stream);
int BitFilePutBit(const int c, bit_file_t *stream);

/* get/put number of bits (most significant bit to least significat bit) */
int BitFileGetBits(bit_file_t *stream, void *bits, const unsigned int count);
int BitFilePutBits(bit_file_t *stream, void *bits, const unsigned int count);

/***************************************************************************
* get/put number of bits from integer types (short, int, long, ...)
* machine endiness is accounted for.
* size is the size of the data structure pointer to by bits.
***************************************************************************/
int BitFileGetBitsInt(bit_file_t *stream, void *bits, const unsigned int count,
    const size_t size);
int BitFilePutBitsInt(bit_file_t *stream, void *bits, const unsigned int count,
    const size_t size);

/***************************************************************************
*          Lempel, Ziv, Storer, and Szymanski Encoding and Decoding
*
*   File    : lzlocal.h
*   Purpose : Internal headers for LZSS encode and decode routines.
*             Contains the prototypes to be used by the different match
*             finding algorithms.
*   Author  : Michael Dipperstein
*   Date    : February 18, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: lzlocal.h,v 1.5 2007/09/20 04:34:25 michael Exp $
*   $Log: lzlocal.h,v $
*   Revision 1.5  2007/09/20 04:34:25  michael
*   Changes required for LGPL v3.
*
*   Revision 1.4  2006/12/26 04:09:09  michael
*   Updated e-mail address and minor text clean-up.
*
*   Revision 1.3  2005/12/28 05:58:52  michael
*   Add Wrap macro to replace mod when value is less than twice the limit.
*
*   Revision 1.2  2004/11/08 05:54:18  michael
*   1. Split encode and decode routines for smarter linking
*   2. Renamed lzsample.c sample.c to match my other samples
*   3. Makefile now builds code as libraries for better LGPL compliance.
*
*   Revision 1.1  2004/02/22 17:32:40  michael
*   Initial revision of header files for sliding window search implementations.
*
*
****************************************************************************
*
* LZSS: An ANSI C LZSS Encoding/Decoding Routine
* Copyright (C) 2004-2007 by
* Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the lzss library.
*
* The lzss library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The lzss library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <limits.h>

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#define FALSE   0
#define TRUE    1

#define OFFSET_BITS     12
#define LENGTH_BITS     4

#if (((1 << (OFFSET_BITS + LENGTH_BITS)) - 1) > UINT_MAX)
#error "Size of encoded data must not exceed the size of an unsigned int"
#endif

/* We want a sliding window*/
#define WINDOW_SIZE     (1 << OFFSET_BITS)

/* maximum match length not encoded and maximum length encoded (4 bits) */
#define MAX_UNCODED     2
#define MAX_CODED       ((1 << LENGTH_BITS) + MAX_UNCODED)

#define ENCODED     0       /* encoded string */
#define UNCODED     1       /* unencoded character */

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

/***************************************************************************
* This data structure stores an encoded string in (offset, length) format.
* The actual encoded string is stored using OFFSET_BITS for the offset and
* LENGTH_BITS for the length.
***************************************************************************/
typedef struct encoded_string_t
{
    unsigned int offset;    /* offset to start of longest match */
    unsigned int length;    /* length of longest match */
} encoded_string_t;

/***************************************************************************
*                                 MACROS
***************************************************************************/
/* wraps array index within array bounds (assumes value < 2 * limit) */
#define Wrap(value, limit)      (((value) < (limit)) ? (value) : ((value) - (limit)))

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
void InitializeSearchStructures(void);

encoded_string_t FindMatch(unsigned int windowHead, unsigned int uncodedHead);

void ReplaceChar(unsigned int charIndex, unsigned char replacement);

/***************************************************************************
*                        Bit Stream File Implementation
*
*   File    : bitfile.c
*   Purpose : This file implements a simple library of I/O functions for
*             files that contain data in sizes that aren't integral bytes.
*             An attempt was made to make the functions in this library
*             analogous to functions provided to manipulate byte streams.
*             The functions contained in this library were created with
*             compression algorithms in mind, but may be suited to other
*             applications.
*   Author  : Michael Dipperstein
*   Date    : January 9, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: bitfile.c,v 1.10 2007/08/26 21:53:48 michael Exp $
*   $Log: bitfile.c,v $
*   Revision 1.10  2007/08/26 21:53:48  michael
*   Changes required for LGPL v3.
*
*   Revision 1.9  2007/07/10 05:34:07  michael
*   Remove ',' after last element in the enum endian_t.
*
*   Revision 1.8  2007/02/06 06:22:07  michael
*   Used trim program to remove trailing spaces.
*
*   Revision 1.7  2006/06/03 19:32:38  michael
*   Corrected error reporetd anonymous.  The allocation of constants used to
*   open underlying read/write/append files did not account for a terminating
*   null.
*
*   Used spell checker to correct spelling.
*
*   Revision 1.6  2005/12/08 06:56:55  michael
*   Minor text corrections.
*
*   Revision 1.5  2005/12/06 15:06:37  michael
*   Added BitFileGetBitsInt and BitFilePutBitsInt for integer types.
*
*   Revision 1.4  2005/06/23 04:34:18  michael
*   Prevent BitfileGetBits/PutBits from accessing an extra byte when given
*   an integral number of bytes.
*
*   Revision 1.3  2004/11/09 14:16:58  michael
*   Added functions to convert open bit_file_t to FILE and to
*   align open bit_file_t to the next byte.
*
*   Revision 1.2  2004/06/15 13:15:58  michael
*   Use incomplete type to hide definition of bitfile structure
*
*   Revision 1.1.1.1  2004/02/09 05:31:42  michael
*   Initial release
*
*
****************************************************************************
*
* Bitfile: Bit stream File I/O Routines
* Copyright (C) 2004-2007 by Michael Dipperstein (mdipper@cs.ucsb.edu)
*
* This file is part of the bit file library.
*
* The bit file library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The bit file library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdlib.h>
#include <errno.h>

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

typedef enum
{
    BF_UNKNOWN_ENDIAN,
    BF_LITTLE_ENDIAN,
    BF_BIG_ENDIAN
} endian_t;

struct bit_file_t
{
    FILE *fp;                   /* file pointer used by stdio functions */
    endian_t endian;            /* endianess of architecture */
    unsigned char bitBuffer;    /* bits waiting to be read/written */
    unsigned char bitCount;     /* number of bits in bitBuffer */
    BF_MODES mode;              /* open for read, write, or append */
};

/* union used to test for endianess */
typedef union
{
    unsigned long word;
    unsigned char bytes[sizeof(unsigned long)];
} endian_test_t;

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
endian_t DetermineEndianess(void);

int BitFilePutBitsLE(bit_file_t *stream, void *bits, const unsigned int count);
int BitFilePutBitsBE(bit_file_t *stream, void *bits, const unsigned int count,
    const size_t size);

int BitFileGetBitsLE(bit_file_t *stream, void *bits, const unsigned int count);
int BitFileGetBitsBE(bit_file_t *stream, void *bits, const unsigned int count,
    const size_t size);

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/***************************************************************************
*   Function   : BitFileOpen
*   Description: This function opens a bit file for reading, writing,
*                or appending.  If successful, a bit_file_t data
*                structure will be allocated and a pointer to the
*                structure will be returned.
*   Parameters : fileName - NULL terminated string containing the name of
*                           the file to be opened.
*                mode - The mode of the file to be opened
*   Effects    : The specified file will be opened and file structure will
*                be allocated.
*   Returned   : Pointer to the bit_file_t structure for the bit file
*                opened, or NULL on failure.  errno will be set for all
*                failure cases.
***************************************************************************/
bit_file_t *BitFileOpen(const char *fileName, const BF_MODES mode)
{
    char modes[3][3] = {"rb", "wb", "ab"};  /* binary modes for fopen */
    bit_file_t *bf;

    bf = (bit_file_t *)malloc(sizeof(bit_file_t));

    if (bf == NULL)
    {
        /* malloc failed */
        errno = ENOMEM;
    }
    else
    {
        bf->fp = fopen(fileName, modes[mode]);

        if (bf->fp == NULL)
        {
            /* fopen failed */
            free(bf);
            bf = NULL;
        }
        else
        {
            /* fopen succeeded fill in remaining bf data */
            bf->bitBuffer = 0;
            bf->bitCount = 0;
            bf->mode = mode;

            /***************************************************************
            *  TO DO: Consider using the last byte in a file to indicate
            * the number of bits in the previous byte that actually have
            * data.  If I do that, I'll need special handling of files
            * opened with a mode of BF_APPEND.
            ***************************************************************/
        }
    }

    bf->endian = DetermineEndianess();
    return (bf);
}

/***************************************************************************
*   Function   : MakeBitFile
*   Description: This function naively wraps a standard file in a
*                bit_file_t structure.  ANSI-C doesn't support file status
*                functions commonly found in other C variants, so the
*                caller must be passed as a parameter.
*   Parameters : stream - pointer to the standard file being wrapped.
*                mode - The mode of the file being wrapped.
*   Effects    : A bit_file_t structure will be created for the stream
*                passed as a parameter.
*   Returned   : Pointer to the bit_file_t structure for the bit file
*                or NULL on failure.  errno will be set for all failure
*                cases.
***************************************************************************/
bit_file_t *MakeBitFile(FILE *stream, const BF_MODES mode)
{
    bit_file_t *bf;

    if (stream == NULL)
    {
        /* can't wrapper empty steam */
        errno = EBADF;
        bf = NULL;
    }
    else
    {
        bf = (bit_file_t *)malloc(sizeof(bit_file_t));

        if (bf == NULL)
        {
            /* malloc failed */
            errno = ENOMEM;
        }
        else
        {
            /* set structure data */
            bf->fp = stream;
            bf->bitBuffer = 0;
            bf->bitCount = 0;
            bf->mode = mode;
        }
    }

    bf->endian = DetermineEndianess();

    return (bf);
}

/***************************************************************************
*   Function   : DetermineEndianess
*   Description: This function determines the endianess of the current
*                hardware architecture.  An unsigned long is set to 1.  If
*                the 1st byte of the unsigned long gets the 1, this is a
*                little endian machine.  If the last byte gets the 1, this
*                is a big endian machine.
*   Parameters : None
*   Effects    : None
*   Returned   : endian_t for current machine architecture
***************************************************************************/
endian_t DetermineEndianess(void)
{
    endian_t endian;
    endian_test_t endianTest;

    endianTest.word = 1;

    if (endianTest.bytes[0] == 1)
    {
        /* LSB is 1st byte (little endian)*/
        endian = BF_LITTLE_ENDIAN;
    }
    else if (endianTest.bytes[sizeof(unsigned long) - 1] == 1)
    {
        /* LSB is last byte (big endian)*/
        endian = BF_BIG_ENDIAN;
    }
    else
    {
        endian = BF_UNKNOWN_ENDIAN;
    }

    return endian;
}

/***************************************************************************
*   Function   : BitFileClose
*   Description: This function closes a bit file and frees all associated
*                data.
*   Parameters : stream - pointer to bit file stream being closed
*   Effects    : The specified file will be closed and the file structure
*                will be freed.
*   Returned   : 0 for success or EOF for failure.
***************************************************************************/
int BitFileClose(bit_file_t *stream)
{
    int returnValue = 0;

    if (stream == NULL)
    {
        return(EOF);
    }

    if ((stream->mode == BF_WRITE) || (stream->mode == BF_APPEND))
    {
        /* write out any unwritten bits */
        if (stream->bitCount != 0)
        {
            (stream->bitBuffer) <<= 8 - (stream->bitCount);
            fputc(stream->bitBuffer, stream->fp);   /* handle error? */
        }
    }

    /***********************************************************************
    *  TO DO: Consider writing an additional byte indicating the number of
    *  valid bits (bitCount) in the previous byte.
    ***********************************************************************/

    /* close file */
    returnValue = fclose(stream->fp);

    /* free memory allocated for bit file */
    free(stream);

    return(returnValue);
}

/***************************************************************************
*   Function   : BitFileToFILE
*   Description: This function flushes and frees the bitfile structure,
*                returning a pointer to a stdio file.
*   Parameters : stream - pointer to bit file stream being closed
*   Effects    : The specified bitfile will be made usable as a stdio
*                FILE.
*   Returned   : Pointer to FILE.  NULL for failure.
***************************************************************************/
FILE *BitFileToFILE(bit_file_t *stream)
{
    FILE *fp = NULL;

    if (stream == NULL)
    {
        return(NULL);
    }

    if ((stream->mode == BF_WRITE) || (stream->mode == BF_APPEND))
    {
        /* write out any unwritten bits */
        if (stream->bitCount != 0)
        {
            (stream->bitBuffer) <<= 8 - (stream->bitCount);
            fputc(stream->bitBuffer, stream->fp);   /* handle error? */
        }
    }

    /***********************************************************************
    *  TO DO: Consider writing an additional byte indicating the number of
    *  valid bits (bitCount) in the previous byte.
    ***********************************************************************/

    /* close file */
    fp = stream->fp;

    /* free memory allocated for bit file */
    free(stream);

    return(fp);
}

/***************************************************************************
*   Function   : BitFileByteAlign
*   Description: This function aligns the bitfile to the nearest byte.  For
*                output files, this means writing out the bit buffer with
*                extra bits set to 0.  For input files, this means flushing
*                the bit buffer.
*   Parameters : stream - pointer to bit file stream to align
*   Effects    : Flushes out the bit buffer.
*   Returned   : EOF if stream is NULL.  Otherwise, the contents of the
*                bit buffer.
***************************************************************************/
int BitFileByteAlign(bit_file_t *stream)
{
    int returnValue;

    if (stream == NULL)
    {
        return(EOF);
    }

    returnValue = stream->bitBuffer;

    if ((stream->mode == BF_WRITE) || (stream->mode == BF_APPEND))
    {
        /* write out any unwritten bits */
        if (stream->bitCount != 0)
        {
            (stream->bitBuffer) <<= 8 - (stream->bitCount);
            fputc(stream->bitBuffer, stream->fp);   /* handle error? */
        }
    }

    stream->bitBuffer = 0;
    stream->bitCount = 0;

    return (returnValue);
}

/***************************************************************************
*   Function   : BitFileGetChar
*   Description: This function returns the next byte from the file passed as
*                a parameter.
*   Parameters : stream - pointer to bit file stream to read from
*   Effects    : Reads next byte from file and updates buffer accordingly.
*   Returned   : EOF if a whole byte cannot be obtained.  Otherwise,
*                the character read.
***************************************************************************/
int BitFileGetChar(bit_file_t *stream)
{
    int returnValue;
    unsigned char tmp;

    if (stream == NULL)
    {
        return(EOF);
    }

    returnValue = fgetc(stream->fp);

    if (stream->bitCount == 0)
    {
        /* we can just get byte from file */
        return returnValue;
    }

    /* we have some buffered bits to return too */
    if (returnValue != EOF)
    {
        /* figure out what to return */
        tmp = ((unsigned char)returnValue) >> (stream->bitCount);
        tmp |= ((stream->bitBuffer) << (8 - (stream->bitCount)));

        /* put remaining in buffer. count shouldn't change. */
        stream->bitBuffer = returnValue;

        returnValue = tmp;
    }

    return returnValue;
}

/***************************************************************************
*   Function   : BitFilePutChar
*   Description: This function writes the byte passed as a parameter to the
*                file passed a parameter.
*   Parameters : c - the character to be written
*                stream - pointer to bit file stream to write to
*   Effects    : Writes a byte to the file and updates buffer accordingly.
*   Returned   : On success, the character written, otherwise EOF.
***************************************************************************/
int BitFilePutChar(const int c, bit_file_t *stream)
{
    unsigned char tmp;

    if (stream == NULL)
    {
        return(EOF);
    }

    if (stream->bitCount == 0)
    {
        /* we can just put byte from file */
        return fputc(c, stream->fp);
    }

    /* figure out what to write */
    tmp = ((unsigned char)c) >> (stream->bitCount);
    tmp = tmp | ((stream->bitBuffer) << (8 - stream->bitCount));

    if (fputc(tmp, stream->fp) != EOF)
    {
        /* put remaining in buffer. count shouldn't change. */
        stream->bitBuffer = c;
    }
    else
    {
        return EOF;
    }

    return tmp;
}

/***************************************************************************
*   Function   : BitFileGetBit
*   Description: This function returns the next bit from the file passed as
*                a parameter.  The bit value returned is the msb in the
*                bit buffer.
*   Parameters : stream - pointer to bit file stream to read from
*   Effects    : Reads next bit from bit buffer.  If the buffer is empty,
*                a new byte will be read from the file.
*   Returned   : 0 if bit == 0, 1 if bit == 1, and EOF if operation fails.
***************************************************************************/
int BitFileGetBit(bit_file_t *stream)
{
    int returnValue;

    if (stream == NULL)
    {
        return(EOF);
    }

    if (stream->bitCount == 0)
    {
        /* buffer is empty, read another character */
        if ((returnValue = fgetc(stream->fp)) == EOF)
        {
            return EOF;
        }
        else
        {
            stream->bitCount = 8;
            stream->bitBuffer = returnValue;
        }
    }

    /* bit to return is msb in buffer */
    stream->bitCount--;
    returnValue = (stream->bitBuffer) >> (stream->bitCount);

    return (returnValue & 0x01);
}

/***************************************************************************
*   Function   : BitFilePutBit
*   Description: This function writes the bit passed as a parameter to the
*                file passed a parameter.
*   Parameters : c - the bit value to be written
*                stream - pointer to bit file stream to write to
*   Effects    : Writes a bit to the bit buffer.  If the buffer has a byte,
*                the buffer is written to the file and cleared.
*   Returned   : On success, the bit value written, otherwise EOF.
***************************************************************************/
int BitFilePutBit(const int c, bit_file_t *stream)
{
    int returnValue = c;

    if (stream == NULL)
    {
        return(EOF);
    }

    stream->bitCount++;
    stream->bitBuffer <<= 1;

    if (c != 0)
    {
        stream->bitBuffer |= 1;
    }

    /* write bit buffer if we have 8 bits */
    if (stream->bitCount == 8)
    {
        if (fputc(stream->bitBuffer, stream->fp) == EOF)
        {
            returnValue = EOF;
        }

        /* reset buffer */
        stream->bitCount = 0;
        stream->bitBuffer = 0;
    }

    return returnValue;
}

/***************************************************************************
*   Function   : BitFileGetBits
*   Description: This function reads the specified number of bits from the
*                file passed as a parameter and writes them to the
*                requested memory location (msb to lsb).
*   Parameters : stream - pointer to bit file stream to read from
*                bits - address to store bits read
*                count - number of bits to read
*   Effects    : Reads bits from the bit buffer and file stream.  The bit
*                buffer will be modified as necessary.
*   Returned   : EOF for failure, otherwise the number of bits read.  If
*                an EOF is reached before all the bits are read, bits
*                will contain every bit through the last complete byte.
***************************************************************************/
int BitFileGetBits(bit_file_t *stream, void *bits, const unsigned int count)
{
    unsigned char *bytes, shifts;
    int offset, remaining, returnValue;

    bytes = (unsigned char *)bits;

    if ((stream == NULL) || (bits == NULL))
    {
        return(EOF);
    }

    offset = 0;
    remaining = count;

    /* read whole bytes */
    while (remaining >= 8)
    {
        returnValue = BitFileGetChar(stream);

        if (returnValue == EOF)
        {
            return EOF;
        }

        bytes[offset] = (unsigned char)returnValue;
        remaining -= 8;
        offset++;
    }

    if (remaining != 0)
    {
        /* read remaining bits */
        shifts = 8 - remaining;

        while (remaining > 0)
        {
            returnValue = BitFileGetBit(stream);

            if (returnValue == EOF)
            {
                return EOF;
            }

            bytes[offset] <<= 1;
            bytes[offset] |= (returnValue & 0x01);
            remaining--;
        }

        /* shift last bits into position */
        bytes[offset] <<= shifts;
    }

    return count;
}

/***************************************************************************
*   Function   : BitFilePutBits
*   Description: This function writes the specified number of bits from the
*                memory location passed as a parameter to the file passed
*                as a parameter.   Bits are written msb to lsb.
*   Parameters : stream - pointer to bit file stream to write to
*                bits - pointer to bits to write
*                count - number of bits to write
*   Effects    : Writes bits to the bit buffer and file stream.  The bit
*                buffer will be modified as necessary.
*   Returned   : EOF for failure, otherwise the number of bits written.  If
*                an error occurs after a partial write, the partially
*                written bits will not be unwritten.
***************************************************************************/
int BitFilePutBits(bit_file_t *stream, void *bits, const unsigned int count)
{
    unsigned char *bytes, tmp;
    int offset, remaining, returnValue;

    bytes = (unsigned char *)bits;

    if ((stream == NULL) || (bits == NULL))
    {
        return(EOF);
    }

    offset = 0;
    remaining = count;

    /* write whole bytes */
    while (remaining >= 8)
    {
        returnValue = BitFilePutChar(bytes[offset], stream);

        if (returnValue == EOF)
        {
            return EOF;
        }

        remaining -= 8;
        offset++;
    }

    if (remaining != 0)
    {
        /* write remaining bits */
        tmp = bytes[offset];

        while (remaining > 0)
        {
            returnValue = BitFilePutBit((tmp & 0x80), stream);

            if (returnValue == EOF)
            {
                return EOF;
            }

            tmp <<= 1;
            remaining--;
        }
    }

    return count;
}

/***************************************************************************
*   Function   : BitFileGetBitsInt
*   Description: This function provides a machine independent layer that
*                allows a single function call to stuff an arbitrary number
*                of bits into an integer type variable.
*   Parameters : stream - pointer to bit file stream to read from
*                bits - address to store bits read
*                count - number of bits to read
*                size - sizeof type containing "bits"
*   Effects    : Calls a function that reads bits from the bit buffer and
*                file stream.  The bit buffer will be modified as necessary.
*                the bits will be written to "bits" from least significant
*                byte to most significant byte.
*   Returned   : EOF for failure, otherwise the number of bits read by the
*                called function.
***************************************************************************/
int BitFileGetBitsInt(bit_file_t *stream, void *bits, const unsigned int count,
    const size_t size)
{
    int returnValue;

    if ((stream == NULL) || (bits == NULL))
    {
        return(EOF);
    }

    if (stream->endian == BF_LITTLE_ENDIAN)
    {
        returnValue = BitFileGetBitsLE(stream, bits, count);
    }
    else if (stream->endian == BF_BIG_ENDIAN)
    {
        returnValue = BitFileGetBitsBE(stream, bits, count, size);
    }
    else
    {
        returnValue = EOF;
    }

    return returnValue;
}

/***************************************************************************
*   Function   : BitFileGetBitsLE   (Little Endian)
*   Description: This function reads the specified number of bits from the
*                file passed as a parameter and writes them to the
*                requested memory location (LSB to MSB).
*   Parameters : stream - pointer to bit file stream to read from
*                bits - address to store bits read
*                count - number of bits to read
*   Effects    : Reads bits from the bit buffer and file stream.  The bit
*                buffer will be modified as necessary.  bits is treated as
*                a little endian integer of length >= (count/8) + 1.
*   Returned   : EOF for failure, otherwise the number of bits read.  If
*                an EOF is reached before all the bits are read, bits
*                will contain every bit through the last successful read.
***************************************************************************/
int BitFileGetBitsLE(bit_file_t *stream, void *bits, const unsigned int count)
{
    unsigned char *bytes, shifts;
    int offset, remaining, returnValue;

    bytes = (unsigned char *)bits;

    offset = 0;
    remaining = count;

    /* read whole bytes */
    while (remaining >= 8)
    {
        returnValue = BitFileGetChar(stream);

        if (returnValue == EOF)
        {
            return EOF;
        }

        bytes[offset] = (unsigned char)returnValue;
        remaining -= 8;
        offset++;
    }

    if (remaining != 0)
    {
        /* read remaining bits */
        shifts = 8 - remaining;

        while (remaining > 0)
        {
            returnValue = BitFileGetBit(stream);

            if (returnValue == EOF)
            {
                return EOF;
            }

            bytes[offset] <<= 1;
            bytes[offset] |= (returnValue & 0x01);
            remaining--;
        }

    }

    return count;
}

/***************************************************************************
*   Function   : BitFileGetBitsBE   (Big Endian)
*   Description: This function reads the specified number of bits from the
*                file passed as a parameter and writes them to the
*                requested memory location (LSB to MSB).
*   Parameters : stream - pointer to bit file stream to read from
*                bits - address to store bits read
*                count - number of bits to read
*                size - sizeof type containing "bits"
*   Effects    : Reads bits from the bit buffer and file stream.  The bit
*                buffer will be modified as necessary.  bits is treated as
*                a big endian integer of length size.
*   Returned   : EOF for failure, otherwise the number of bits read.  If
*                an EOF is reached before all the bits are read, bits
*                will contain every bit through the last successful read.
***************************************************************************/
int BitFileGetBitsBE(bit_file_t *stream, void *bits, const unsigned int count,
    const size_t size)
{
    unsigned char *bytes, shifts;
    int offset, remaining, returnValue;

    if (count > (size * 8))
    {
        /* too many bits to read */
        return EOF;
    }

    bytes = (unsigned char *)bits;

    offset = size - 1;
    remaining = count;

    /* read whole bytes */
    while (remaining >= 8)
    {
        returnValue = BitFileGetChar(stream);

        if (returnValue == EOF)
        {
            return EOF;
        }

        bytes[offset] = (unsigned char)returnValue;
        remaining -= 8;
        offset--;
    }

    if (remaining != 0)
    {
        /* read remaining bits */
        shifts = 8 - remaining;

        while (remaining > 0)
        {
            returnValue = BitFileGetBit(stream);

            if (returnValue == EOF)
            {
                return EOF;
            }

            bytes[offset] <<= 1;
            bytes[offset] |= (returnValue & 0x01);
            remaining--;
        }

    }

    return count;
}

/***************************************************************************
*   Function   : BitFilePutBitsInt
*   Description: This function provides a machine independent layer that
*                allows a single function call to write an arbitrary number
*                of bits from an integer type variable into a file.
*   Parameters : stream - pointer to bit file stream to write to
*                bits - pointer to bits to write
*                count - number of bits to write
*                size - sizeof type containing "bits"
*   Effects    : Calls a function that writes bits to the bit buffer and
*                file stream.  The bit buffer will be modified as necessary.
*                the bits will be written to the file stream from least
*                significant byte to most significant byte.
*   Returned   : EOF for failure, otherwise the number of bits written.  If
*                an error occurs after a partial write, the partially
*                written bits will not be unwritten.
***************************************************************************/
int BitFilePutBitsInt(bit_file_t *stream, void *bits, const unsigned int count,
    const size_t size)
{
    int returnValue;

    if ((stream == NULL) || (bits == NULL))
    {
        return(EOF);
    }

    if (stream->endian == BF_LITTLE_ENDIAN)
    {
        returnValue = BitFilePutBitsLE(stream, bits, count);
    }
    else if (stream->endian == BF_BIG_ENDIAN)
    {
        returnValue = BitFilePutBitsBE(stream, bits, count, size);
    }
    else
    {
        returnValue = EOF;
    }

    return returnValue;
}

/***************************************************************************
*   Function   : BitFilePutBitsLE   (Little Endian)
*   Description: This function writes the specified number of bits from the
*                memory location passed as a parameter to the file passed
*                as a parameter.   Bits are written LSB to MSB.
*   Parameters : stream - pointer to bit file stream to write to
*                bits - pointer to bits to write
*                count - number of bits to write
*   Effects    : Writes bits to the bit buffer and file stream.  The bit
*                buffer will be modified as necessary.  bits is treated as
*                a little endian integer of length >= (count/8) + 1.
*   Returned   : EOF for failure, otherwise the number of bits written.  If
*                an error occurs after a partial write, the partially
*                written bits will not be unwritten.
***************************************************************************/
int BitFilePutBitsLE(bit_file_t *stream, void *bits, const unsigned int count)
{
    unsigned char *bytes, tmp;
    int offset, remaining, returnValue;

    bytes = (unsigned char *)bits;
    offset = 0;
    remaining = count;

    /* write whole bytes */
    while (remaining >= 8)
    {
        returnValue = BitFilePutChar(bytes[offset], stream);

        if (returnValue == EOF)
        {
            return EOF;
        }

        remaining -= 8;
        offset++;
    }

    if (remaining != 0)
    {
        /* write remaining bits */
        tmp = bytes[offset];
        tmp <<= (8 - remaining);

        while (remaining > 0)
        {
            returnValue = BitFilePutBit((tmp & 0x80), stream);

            if (returnValue == EOF)
            {
                return EOF;
            }

            tmp <<= 1;
            remaining--;
        }
    }

    return count;
}

/***************************************************************************
*   Function   : BitFilePutBitsBE   (Big Endian)
*   Description: This function writes the specified number of bits from the
*                memory location passed as a parameter to the file passed
*                as a parameter.   Bits are written LSB to MSB.
*   Parameters : stream - pointer to bit file stream to write to
*                bits - pointer to bits to write
*                count - number of bits to write
*   Effects    : Writes bits to the bit buffer and file stream.  The bit
*                buffer will be modified as necessary.  bits is treated as
*                a big endian integer of length size.
*   Returned   : EOF for failure, otherwise the number of bits written.  If
*                an error occurs after a partial write, the partially
*                written bits will not be unwritten.
***************************************************************************/
int BitFilePutBitsBE(bit_file_t *stream, void *bits, const unsigned int count,
    const size_t size)
{
    unsigned char *bytes, tmp;
    int offset, remaining, returnValue;

    if (count > (size * 8))
    {
        /* too many bits to write */
        return EOF;
    }

    bytes = (unsigned char *)bits;
    offset = size - 1;
    remaining = count;

    /* write whole bytes */
    while (remaining >= 8)
    {
        returnValue = BitFilePutChar(bytes[offset], stream);

        if (returnValue == EOF)
        {
            return EOF;
        }

        remaining -= 8;
        offset--;
    }

    if (remaining != 0)
    {
        /* write remaining bits */
        tmp = bytes[offset];
        tmp <<= (8 - remaining);

        while (remaining > 0)
        {
            returnValue = BitFilePutBit((tmp & 0x80), stream);

            if (returnValue == EOF)
            {
                return EOF;
            }

            tmp <<= 1;
            remaining--;
        }
    }

    return count;
}

/***************************************************************************
*                 Lempel, Ziv, Storer, and Szymanski Decoding
*
*   File    : lzdecode.c
*   Purpose : Use lzss coding (Storer and Szymanski's modified LZ77) to
*             decompress lzss  encoded files.
*   Author  : Michael Dipperstein
*   Date    : November 07, 2004
*
****************************************************************************
*   UPDATES
*
*   Date        Change
*   12/10/03    Changed handling of sliding window to better match standard
*               algorithm description.
*   12/11/03    Remebered to copy encoded characters to the sliding window
*               even when there are no more characters in the input stream.
*
*
*   Revision 1.2  2004/02/22 17:14:26  michael
*   - Separated encode/decode, match finding, and main.
*   - Use bitfiles for reading/writing files
*   - Use traditional LZSS encoding where the coded/uncoded bits
*     precede the symbol they are associated with, rather than
*     aggregating the bits.
*
*   Revision 1.1.1.1  2004/01/21 06:25:49  michael
*   Initial version
*
*   11/07/04    Separated encode and decode functions for improved
*               modularity.
*
*   $Id: lzdecode.c,v 1.7 2007/09/20 04:34:25 michael Exp $
*   $Log: lzdecode.c,v $
*   Revision 1.7  2007/09/20 04:34:25  michael
*   Changes required for LGPL v3.
*
*   Revision 1.6  2007/03/25 05:11:32  michael
*   Corrected file closure error reported by "Carl@Yahoo" .  Now  both input
*   and output files are closed.
*
*   Revision 1.5  2006/12/26 04:09:09  michael
*   Updated e-mail address and minor text clean-up.
*
*   Revision 1.4  2006/12/26 02:05:00  michael
*   Corrected bug identified by Andrej Sinicyn which resulted in stdin being
*   used as the default output for decoded data.
*
*   Revision 1.3  2005/12/28 06:03:30  michael
*   Use slower but clearer Get/PutBitsInt for reading/writing bits.
*   Replace mod with conditional Wrap macro.
*
*   Revision 1.2  2004/11/13 22:51:00  michael
*   Provide distinct names for by file and by name functions and add some
*   comments to make their usage clearer.
*
*   Revision 1.1  2004/11/08 05:54:18  michael
*   1. Split encode and decode routines for smarter linking
*   2. Renamed lzsample.c sample.c to match my other samples
*   3. Makefile now builds code as libraries for better LGPL compliance.
*
*
*
****************************************************************************
*
* LZDecode: An ANSI C LZSS Decoding Routines
* Copyright (C) 2003-2007 by
* Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the lzss library.
*
* The lzss library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The lzss library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

/***************************************************************************
*                                CONSTANTS
***************************************************************************/

/***************************************************************************
*                            GLOBAL VARIABLES
***************************************************************************/
/* cyclic buffer sliding window of already read characters */
extern unsigned char slidingWindow[];
extern unsigned char uncodedLookahead[];

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : DecodeLZSSByFile
*   Description: This function will read an LZSS encoded input file and
*                write an output file.  This algorithm encodes strings as 16
*                bits (a 12 bit offset + a 4 bit length).
*   Parameters : fpIn - pointer to the open binary file to decode
*                fpOut - pointer to the open binary file to write decoded
*                       output
*   Effects    : fpIn is decoded and written to fpOut.  Neither file is
*                closed after exit.
*   Returned   : EXIT_SUCCESS or EXIT_FAILURE
****************************************************************************/
int DecodeLZSSByFile(FILE *fpIn, FILE *fpOut)
{
    bit_file_t *bfpIn;

    int c;
    unsigned int i, nextChar;
    encoded_string_t code;              /* offset/length code for string */

    if (fpIn == NULL)
    {
        /* use stdin if no input file */
        bfpIn = MakeBitFile(stdin, BF_READ);
    }
    else
    {
        /* convert output file to bitfile */
        bfpIn = MakeBitFile(fpIn, BF_READ);
    }

    /* use stdout if no output file */
    if (fpOut == NULL)
    {
        fpOut = stdout;
    }

    /************************************************************************
    * Fill the sliding window buffer with some known vales.  EncodeLZSS must
    * use the same values.  If common characters are used, there's an
    * increased chance of matching to the earlier strings.
    ************************************************************************/
    memset(slidingWindow, ' ', WINDOW_SIZE * sizeof(unsigned char));

    nextChar = 0;

    while (TRUE)
    {
        if ((c = BitFileGetBit(bfpIn)) == EOF)
        {
            /* we hit the EOF */
            break;
        }

        if (c == UNCODED)
        {
            /* uncoded character */
            if ((c = BitFileGetChar(bfpIn)) == EOF)
            {
                break;
            }

            /* write out byte and put it in sliding window */
            putc(c, fpOut);
            slidingWindow[nextChar] = c;
            nextChar = Wrap((nextChar + 1), WINDOW_SIZE);
        }
        else
        {
            /* offset and length */
            code.offset = 0;
            code.length = 0;

            if ((BitFileGetBitsInt(bfpIn, &code.offset, OFFSET_BITS,
                sizeof(unsigned int))) == EOF)
            {
                break;
            }

            if ((BitFileGetBitsInt(bfpIn, &code.length, LENGTH_BITS,
                sizeof(unsigned int))) == EOF)
            {
                break;
            }

            code.length += MAX_UNCODED + 1;

            /****************************************************************
            * Write out decoded string to file and lookahead.  It would be
            * nice to write to the sliding window instead of the lookahead,
            * but we could end up overwriting the matching string with the
            * new string if abs(offset - next char) < match length.
            ****************************************************************/
            for (i = 0; i < code.length; i++)
            {
                c = slidingWindow[Wrap((code.offset + i), WINDOW_SIZE)];
                putc(c, fpOut);
                uncodedLookahead[i] = c;
            }

            /* write out decoded string to sliding window */
            for (i = 0; i < code.length; i++)
            {
                slidingWindow[(nextChar + i) % WINDOW_SIZE] =
                    uncodedLookahead[i];
            }

            nextChar = Wrap((nextChar + code.length), WINDOW_SIZE);
        }
    }

    /* we've decoded everything, free bitfile structure */
    BitFileToFILE(bfpIn);

    return (EXIT_SUCCESS);
}

/***************************************************************************
*                 Lempel, Ziv, Storer, and Szymanski Encoding
*
*   File    : lzdecode.c
*   Purpose : Use lzss coding (Storer and Szymanski's modified LZ77) to
*             compress lzss data files.
*   Author  : Michael Dipperstein
*   Date    : November 07, 2004
*
****************************************************************************
*   UPDATES
*
*   Date        Change
*   12/10/03    Changed handling of sliding window to better match standard
*               algorithm description.
*   12/11/03    Remebered to copy encoded characters to the sliding window
*               even when there are no more characters in the input stream.
*
*
*   Revision 1.2  2004/02/22 17:14:26  michael
*   - Separated encode/decode, match finding, and main.
*   - Use bitfiles for reading/writing files
*   - Use traditional LZSS encoding where the coded/uncoded bits
*     precede the symbol they are associated with, rather than
*     aggregating the bits.
*
*   Revision 1.1.1.1  2004/01/21 06:25:49  michael
*   Initial version
*
*   11/07/04    Separated encode and decode functions for improved
*               modularity.
*
*   $Id: lzencode.c,v 1.6 2007/09/20 04:34:25 michael Exp $
*   $Log: lzencode.c,v $
*   Revision 1.6  2007/09/20 04:34:25  michael
*   Changes required for LGPL v3.
*
*   Revision 1.5  2007/03/25 05:11:32  michael
*   Corrected file closure error reported by "Carl@Yahoo" .  Now  both input
*   and output files are closed.
*
*   Revision 1.4  2006/12/26 04:09:09  michael
*   Updated e-mail address and minor text clean-up.
*
*   Revision 1.3  2005/12/28 06:03:30  michael
*   Use slower but clearer Get/PutBitsInt for reading/writing bits.
*   Replace mod with conditional Wrap macro.
*
*   Revision 1.2  2004/11/13 22:51:00  michael
*   Provide distinct names for by file and by name functions and add some
*   comments to make their usage clearer.
*
*   Revision 1.1  2004/11/08 05:54:18  michael
*   1. Split encode and decode routines for smarter linking
*   2. Renamed lzsample.c sample.c to match my other samples
*   3. Makefile now builds code as libraries for better LGPL compliance.
*
*
*
****************************************************************************
*
* LZEncode: An ANSI C LZSS Encoding Routines
* Copyright (C) 2003-2007 by
* Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the lzss library.
*
* The lzss library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The lzss library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

/***************************************************************************
*                                CONSTANTS
***************************************************************************/

/***************************************************************************
*                            GLOBAL VARIABLES
***************************************************************************/
/* cyclic buffer sliding window of already read characters */
extern unsigned char slidingWindow[];
extern unsigned char uncodedLookahead[];

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : EncodeLZSSByFile
*   Description: This function will read an input file and write an output
*                file encoded according to the traditional LZSS algorithm.
*                This algorithm encodes strings as 16 bits (a 12 bit offset
*                + a 4 bit length).
*   Parameters : fpIn - pointer to the open binary file to encode
*                fpOut - pointer to the open binary file to write encoded
*                       output
*   Effects    : fpIn is encoded and written to fpOut.  Neither file is
*                closed after exit.
*   Returned   : EXIT_SUCCESS or EXIT_FAILURE
****************************************************************************/
int EncodeLZSSByFile(FILE *fpIn, FILE *fpOut)
{
    bit_file_t *bfpOut;

    encoded_string_t matchData;
    unsigned int i, c;
    unsigned int len;                       /* length of string */

    /* head of sliding window and lookahead */
    unsigned int windowHead, uncodedHead;

    /* use stdin if no input file */
    if (fpIn == NULL)
    {
        fpIn = stdin;
    }

    if (fpOut == NULL)
    {
        /* use stdout if no output file */
        bfpOut = MakeBitFile(stdout, BF_WRITE);
    }
    else
    {
        /* convert output file to bitfile */
        bfpOut = MakeBitFile(fpOut, BF_WRITE);
    }

    windowHead = 0;
    uncodedHead = 0;

    /************************************************************************
    * Fill the sliding window buffer with some known vales.  DecodeLZSS must
    * use the same values.  If common characters are used, there's an
    * increased chance of matching to the earlier strings.
    ************************************************************************/
    memset(slidingWindow, ' ', WINDOW_SIZE * sizeof(unsigned char));

    /************************************************************************
    * Copy MAX_CODED bytes from the input file into the uncoded lookahead
    * buffer.
    ************************************************************************/
    for (len = 0; len < MAX_CODED && (c = getc(fpIn)) != EOF; len++)
    {
        uncodedLookahead[len] = c;
    }

    if (len == 0)
    {
        return (EXIT_SUCCESS);   /* inFile was empty */
    }

    /* Look for matching string in sliding window */
    InitializeSearchStructures();
    matchData = FindMatch(windowHead, uncodedHead);

    /* now encoded the rest of the file until an EOF is read */
    while (len > 0)
    {
        if (matchData.length > len)
        {
            /* garbage beyond last data happened to extend match length */
            matchData.length = len;
        }

        if (matchData.length <= MAX_UNCODED)
        {
            /* not long enough match.  write uncoded flag and character */
            BitFilePutBit(UNCODED, bfpOut);
            BitFilePutChar(uncodedLookahead[uncodedHead], bfpOut);

            matchData.length = 1;   /* set to 1 for 1 byte uncoded */
        }
        else
        {
            unsigned int adjustedLen;

            /* adjust the length of the match so minimun encoded len is 0*/
            adjustedLen = matchData.length - (MAX_UNCODED + 1);

            /* match length > MAX_UNCODED.  Encode as offset and length. */
            BitFilePutBit(ENCODED, bfpOut);
            BitFilePutBitsInt(bfpOut, &matchData.offset, OFFSET_BITS,
                sizeof(unsigned int));
            BitFilePutBitsInt(bfpOut, &adjustedLen, LENGTH_BITS,
                sizeof(unsigned int));
        }

        /********************************************************************
        * Replace the matchData.length worth of bytes we've matched in the
        * sliding window with new bytes from the input file.
        ********************************************************************/
        i = 0;
        while ((i < matchData.length) && ((c = getc(fpIn)) != EOF))
        {
            /* add old byte into sliding window and new into lookahead */
            ReplaceChar(windowHead, uncodedLookahead[uncodedHead]);
            uncodedLookahead[uncodedHead] = c;
            windowHead = Wrap((windowHead + 1), WINDOW_SIZE);
            uncodedHead = Wrap((uncodedHead + 1), MAX_CODED);
            i++;
        }

        /* handle case where we hit EOF before filling lookahead */
        while (i < matchData.length)
        {
            ReplaceChar(windowHead, uncodedLookahead[uncodedHead]);
            /* nothing to add to lookahead here */
            windowHead = Wrap((windowHead + 1), WINDOW_SIZE);
            uncodedHead = Wrap((uncodedHead + 1), MAX_CODED);
            len--;
            i++;
        }

        /* find match for the remaining characters */
        matchData = FindMatch(windowHead, uncodedHead);
    }

    /* we've decoded everything, free bitfile structure */
    BitFileToFILE(bfpOut);

   return (EXIT_SUCCESS);
}

/****************************************************************************
*   Function   : EncodeLZSSByName
*   Description: This function is provided to maintain compatibility with
*                older versions of my LZSS implementation which used file
*                names instead of file pointers.
*   Parameters : inFile - name of file to encode
*                outFile - name of file to write encoded output
*   Effects    : inFile is encoded and written to outFile
*   Returned   : EXIT_SUCCESS or EXIT_FAILURE
****************************************************************************/
int EncodeLZSSByName(char *inFile, char *outFile)
{
    FILE *fpIn, *fpOut;
    int returnValue;

    /* open binary input and output files */
    if (inFile == NULL)
    {
        fpIn = stdin;
    }
    if ((fpIn = fopen(inFile, "rb")) == NULL)
    {
        perror(inFile);
        return (EXIT_FAILURE);
    }

    if (outFile == NULL)
    {
        fpOut = stdout;
    }
    else
    {
        if ((fpOut = fopen(outFile, "wb")) == NULL)
        {
            fclose(fpIn);
            perror(outFile);
            return (EXIT_FAILURE);
        }
    }

    returnValue = EncodeLZSSByFile(fpIn, fpOut);

    /* close files */
    fclose(fpIn);
    fclose(fpOut);

    return (returnValue);
}


/****************************************************************************
*   Function   : DecodeLZSSByName
*   Description: This function is provided to maintain compatibility with
*                older versions of my LZSS implementation which used file
*                names instead of file pointers.
*   Parameters : inFile - name of file to decode
*                outFile - name of file to write decoded output
*   Effects    : inFile is decoded and written to outFile
*   Returned   : EXIT_SUCCESS or EXIT_FAILURE
****************************************************************************/
int DecodeLZSSByName(char *inFile, char *outFile)
{
    FILE *fpIn, *fpOut;
    int returnValue;

    if (inFile == NULL)
    {
        fpIn = stdin;
    }
    if ((fpIn = fopen(inFile, "rb")) == NULL)
    {
        perror(inFile);
        return (EXIT_FAILURE);
    }

    if (outFile == NULL)
    {
        fpOut = stdout;
    }
    else
    {
        if ((fpOut = fopen(outFile, "wb")) == NULL)
        {
            fclose(fpIn);
            perror(outFile);
            return (EXIT_FAILURE);
        }
    }

    returnValue = DecodeLZSSByFile(fpIn, fpOut);

    /* close files */
    fclose(fpIn);
    fclose(fpOut);

    return (returnValue);
}

/***************************************************************************
*          Lempel, Ziv, Storer, and Szymanski Encoding and Decoding
*
*   File    : lzss.h
*   Purpose : Header for LZSS encode and decode routines.  Contains the
*             prototypes to be used by programs linking to the LZSS
*             library.
*   Author  : Michael Dipperstein
*   Date    : February 21, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: lzss.h,v 1.5 2007/09/20 04:34:25 michael Exp $
*   $Log: lzss.h,v $
*   Revision 1.5  2007/09/20 04:34:25  michael
*   Changes required for LGPL v3.
*
*   Revision 1.4  2006/12/26 04:09:09  michael
*   Updated e-mail address and minor text clean-up.
*
*   Revision 1.3  2004/11/13 22:51:00  michael
*   Provide distinct names for by file and by name functions and add some
*   comments to make their usage clearer.
*
*   Revision 1.2  2004/11/08 05:54:18  michael
*   1. Split encode and decode routines for smarter linking
*   2. Renamed lzsample.c sample.c to match my other samples
*   3. Makefile now builds code as libraries for better LGPL compliance.
*
*   Revision 1.1  2004/02/22 17:37:50  michael
*   Initial revision of headers for encode and decode routines.
*
*
****************************************************************************
*
* LZSS: An ANSI C LZSS Encoding/Decoding Routine
* Copyright (C) 2004, 2006, 2007 by
* Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the lzss library.
*
* The lzss library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The lzss library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/
#ifndef _LZSS_H
#define _LZSS_H

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>

/***************************************************************************
*                                 MACROS
***************************************************************************/
/* macros for compatibility with older library */
#define EncodeLZSS(in, out)     EncodeLZSSByName((in), (out))
#define DecodeLZSS(in, out)     DecodeLZSSByName((in), (out))

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
/***************************************************************************
* LZSS encoding and decoding prototypes for functions with file name
* parameters.  Provide these functions with name of the file to be
* encoded/decoded (inFile) and the name of the target file (outFile).
* These functions return EXIT_SUCCESS or EXIT_FAILURE.
***************************************************************************/
int EncodeLZSSByName(char *inFile, char *outFile);
int DecodeLZSSByName(char *inFile, char *outFile);

/***************************************************************************
* LZSS encoding and decoding prototypes for functions with file pointer
* parameters.  Provide these functions with a pointer to the open binary
* file to be encoded/decoded (fpIn) and pointer to the open binary target
* file (fpOut).  It is the job of the function caller to open the files
* prior to callings these functions and to close the file after these
* functions have been called.
* These functions return EXIT_SUCCESS or EXIT_FAILURE.
***************************************************************************/
int EncodeLZSSByFile(FILE *fpIn, FILE *fpOut);
int DecodeLZSSByFile(FILE *fpIn, FILE *fpOut);

#endif      /* ndef _LZSS_H */

/***************************************************************************
*          Lempel, Ziv, Storer, and Szymanski Encoding and Decoding
*
*   File    : hash.c
*   Purpose : Implement hash table optimized matching of uncoded strings
*             for LZSS algorithm.
*   Author  : Michael Dipperstein
*   Date    : February 21, 2004
*
****************************************************************************
*   UPDATES
*   $Id: hash.c,v 1.7 2007/09/20 04:34:25 michael Exp $
*   $Log: hash.c,v $
*   Revision 1.7  2007/09/20 04:34:25  michael
*   Changes required for LGPL v3.
*
*   Revision 1.6  2006/12/26 04:09:09  michael
*   Updated e-mail address and minor text clean-up.
*
*   Revision 1.5  2005/12/29 14:37:56  michael
*   Remove debug statements.
*
*   Revision 1.4  2005/12/29 14:32:56  michael
*   Correct algorithm for replacing characters in dictionary.
*
*   Revision 1.3  2005/12/29 07:06:24  michael
*   Let the hash table size vary with the size of the sliding window.
*
*   Revision 1.2  2005/12/28 06:03:30  michael
*   Use slower but clearer Get/PutBitsInt for reading/writing bits.
*   Replace mod with conditional Wrap macro.
*
*   Revision 1.1  2004/02/22 17:23:02  michael
*   Initial revision of hash table search.  Mostly code from lzhash.c.
*
*
****************************************************************************
*
* Hash: Hash table optimized matching routines used by LZSS
*       Encoding/Decoding Routine
* Copyright (C) 2004-2007 by
* Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the lzss library.
*
* The lzss library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The lzss library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#define NULL_INDEX      (WINDOW_SIZE + 1)

#define HASH_SIZE       (WINDOW_SIZE >> 2)  /* size of hash table */

/***************************************************************************
*                            GLOBAL VARIABLES
***************************************************************************/
/* cyclic buffer sliding window of already read characters */
extern unsigned char slidingWindow[];
extern unsigned char uncodedLookahead[];

unsigned int hashTable[HASH_SIZE];          /* list head for each hash key */
unsigned int next[WINDOW_SIZE];             /* indices of next in hash list */

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : HashKey
*   Description: This function generates a hash key for a (MAX_UNCODED + 1)
*                long string located either in the sliding window or the
*                uncoded lookahead.  The key generation algorithm is
*                supposed to be based on the algorithm used by gzip.  As
*                reported in K. Sadakane, H. Imai. "Improving the Speed of
*                LZ77 Compression by Hashing and Suffix Sorting". IEICE
*                Trans. Fundamentals, Vol. E83-A, No. 12 (December 2000)
*   Parameters : offset - offset into either the uncoded lookahead or the
*                         sliding window.
*                lookahead - TRUE iff offset is an offset into the uncoded
*                            lookahead buffer.
*   Effects    : NONE
*   Returned   : The sliding window index where the match starts and the
*                length of the match.  If there is no match a length of
*                zero will be returned.
****************************************************************************/
unsigned int HashKey(unsigned int offset, unsigned char lookahead)
{
    int i, hashKey;

    hashKey = 0;

    if (lookahead)
    {
        /* string is in the lookahead buffer */
        for (i = 0; i < (MAX_UNCODED + 1); i++)
        {
            hashKey = (hashKey << 5) ^ (uncodedLookahead[offset]);
            hashKey %= HASH_SIZE;
            offset = Wrap((offset + 1), MAX_CODED);
        }
    }
    else
    {
        /* string is in the sliding window */
        for (i = 0; i < (MAX_UNCODED + 1); i++)
        {
            hashKey = (hashKey << 5) ^ (slidingWindow[offset]);
            hashKey %= HASH_SIZE;
            offset = Wrap((offset + 1), WINDOW_SIZE);
        }
    }

    return hashKey;
}

/****************************************************************************
*   Function   : InitializeSearchStructures
*   Description: This function initializes structures used to speed up the
*                process of mathcing uncoded strings to strings in the
*                sliding window.  For hashed searches, this means that a
*                hash table pointing to linked lists is initialized.
*   Parameters : None
*   Effects    : The hash table and next array are initialized.
*   Returned   : None
*
*   NOTE: This function assumes that the sliding window is initially filled
*         with all identical characters.
****************************************************************************/
void InitializeSearchStructures()
{
    unsigned int i;

    /************************************************************************
    * Since the encode routine only fills the sliding window with one
    * character, there is only one hash key for the entier sliding window.
    * That means all positions are in the same linked list.
    ************************************************************************/
    for (i = 0; i < (WINDOW_SIZE - 1); i++)
    {
        next[i] = i + 1;
    }

    /* there's no next for the last character */
    next[WINDOW_SIZE - 1] = NULL_INDEX;

    /* the only list right now is the "   " list */
    for (i = 0; i < HASH_SIZE; i++)
    {
        hashTable[i] = NULL_INDEX;
    }

    hashTable[HashKey(0, FALSE)] = 0;
}

/****************************************************************************
*   Function   : FindMatch
*   Description: This function will search through the slidingWindow
*                dictionary for the longest sequence matching the MAX_CODED
*                long string stored in uncodedLookahead.
*   Parameters : uncodedHead - head of uncoded lookahead buffer
*   Effects    : NONE
*   Returned   : The sliding window index where the match starts and the
*                length of the match.  If there is no match a length of
*                zero will be returned.
****************************************************************************/
encoded_string_t FindMatch(unsigned int windowHead, unsigned int uncodedHead)
{
    encoded_string_t matchData;
    unsigned int i, j;

    matchData.length = 0;
    matchData.offset = 0;

    i = hashTable[HashKey(uncodedHead, TRUE)];  /* start of proper list */
    j = 0;

    while (i != NULL_INDEX)
    {
        if (slidingWindow[i] == uncodedLookahead[uncodedHead])
        {
            /* we matched one how many more match? */
            j = 1;

            while(slidingWindow[Wrap((i + j), WINDOW_SIZE)] ==
                uncodedLookahead[Wrap((uncodedHead + j), MAX_CODED)])
            {
                if (j >= MAX_CODED)
                {
                    break;
                }
                j++;
            }

            if (j > matchData.length)
            {
                matchData.length = j;
                matchData.offset = i;
            }
        }

        if (j >= MAX_CODED)
        {
            matchData.length = MAX_CODED;
            break;
        }

        i = next[i];    /* try next in list */
    }

    return matchData;
}

/****************************************************************************
*   Function   : AddString
*   Description: This function adds the (MAX_UNCODED + 1) long string
*                starting at slidingWindow[charIndex] to the hash table's
*                linked list associated with its hash key.
*   Parameters : charIndex - sliding window index of the string to be
*                            added to the linked list.
*   Effects    : The string starting at slidingWindow[charIndex] is appended
*                to the end of the appropriate linked list.
*   Returned   : NONE
****************************************************************************/
void AddString(unsigned int charIndex)
{
    unsigned int i, hashKey;

    /* inserted character will be at the end of the list */
    next[charIndex] = NULL_INDEX;

    hashKey = HashKey(charIndex, FALSE);

    if (hashTable[hashKey] == NULL_INDEX)
    {
        /* this is the only character in it's list */
        hashTable[hashKey] = charIndex;
        return;
    }

    /* find the end of the list */
    i = hashTable[hashKey];

    while(next[i] != NULL_INDEX)
    {
        i = next[i];
    }

    /* add new character to the list end */
    next[i] = charIndex;
}

/****************************************************************************
*   Function   : RemoveString
*   Description: This function removes the (MAX_UNCODED + 1) long string
*                starting at slidingWindow[charIndex] from the hash table's
*                linked list associated with its hash key.
*   Parameters : charIndex - sliding window index of the string to be
*                            removed from the linked list.
*   Effects    : The string starting at slidingWindow[charIndex] is removed
*                from its linked list.
*   Returned   : NONE
****************************************************************************/
void RemoveString(unsigned int charIndex)
{
    unsigned int i, hashKey;
    unsigned int nextIndex;

    nextIndex = next[charIndex];        /* remember where this points to */
    next[charIndex] = NULL_INDEX;

    hashKey = HashKey(charIndex, FALSE);

    if (hashTable[hashKey] == charIndex)
    {
        /* we're deleting a list head */
        hashTable[hashKey] = nextIndex;
        return;
    }

    /* find character pointing to ours */
    i = hashTable[hashKey];

    while(next[i] != charIndex)
    {
        i = next[i];
    }

    /* point the previous next */
    next[i] = nextIndex;
}

/****************************************************************************
*   Function   : ReplaceChar
*   Description: This function replaces the character stored in
*                slidingWindow[charIndex] with the one specified by
*                replacement.  The hash table entries effected by the
*                replacement are also corrected.
*   Parameters : charIndex - sliding window index of the character to be
*                            removed from the linked list.
*   Effects    : slidingWindow[charIndex] is replaced by replacement.  Old
*                hash entries for strings containing slidingWindow[charIndex]
*                are removed and new ones are added.
*   Returned   : NONE
****************************************************************************/
void ReplaceChar(unsigned int charIndex, unsigned char replacement)
{
    unsigned int firstIndex, i;

    if (charIndex < MAX_UNCODED)
    {
        firstIndex = (WINDOW_SIZE + charIndex) - MAX_UNCODED;
    }
    else
    {
        firstIndex = charIndex - MAX_UNCODED;
    }

    /* remove all hash entries containing character at char index */
    for (i = 0; i < (MAX_UNCODED + 1); i++)
    {
        RemoveString(Wrap((firstIndex + i), WINDOW_SIZE));
    }

    slidingWindow[charIndex] = replacement;

    /* add all hash entries containing character at char index */
    for (i = 0; i < (MAX_UNCODED + 1); i++)
    {
        AddString(Wrap((firstIndex + i), WINDOW_SIZE));
    }
}


/***************************************************************************
*            Lempel, Ziv, Storer, and Szymanski Global Variables
*
*   File    : lzvars.c
*   Purpose : Declare global variables used lzss coding
*             compress/decompress files.
*   Author  : Michael Dipperstein
*   Date    : November 07, 2003
*
****************************************************************************
*   UPDATES
*
*   $Id: lzvars.c,v 1.3 2007/09/20 04:34:25 michael Exp $
*   $Log: lzvars.c,v $
*   Revision 1.3  2007/09/20 04:34:25  michael
*   Changes required for LGPL v3.
*
*   Revision 1.2  2006/12/26 04:09:09  michael
*   Updated e-mail address and minor text clean-up.
*
*   Revision 1.1  2004/11/08 05:54:18  michael
*   1. Split encode and decode routines for smarter linking
*   2. Renamed lzsample.c sample.c to match my other samples
*   3. Makefile now builds code as libraries for better LGPL compliance.
*
*
****************************************************************************
*
* LZvars: Global variables used in ANSI C LZSS Encoding/Decoding Routines
* Copyright (C) 2003, 2004, 2006, 2007 by
* Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the lzss library.
*
* The lzss library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The lzss library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

/***************************************************************************
*                                CONSTANTS
***************************************************************************/

/***************************************************************************
*                            GLOBAL VARIABLES
***************************************************************************/
/* cyclic buffer sliding window of already read characters */
unsigned char slidingWindow[WINDOW_SIZE];
unsigned char uncodedLookahead[MAX_CODED];

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/***************************************************************************
*                     Sample Program Using LZSS Library
*
*   File    : decomp.c
*   Purpose : Demonstrate usage of LZSS library to decompress a file
*   Author  : Michael Dipperstein
*   Date    : November 7, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: decomp.c,v 1.3 2007/09/20 04:34:25 michael Exp $
*   $Log: decomp.c,v $
*   Revision 1.3  2007/09/20 04:34:25  michael
*   Changes required for LGPL v3.
*
*   Revision 1.2  2006/12/26 04:09:09  michael
*   Updated e-mail address and minor text clean-up.
*
*   Revision 1.1  2004/11/08 06:06:07  michael
*   Initial revision of super simple compression and decompression samples.
*
*
****************************************************************************
*
* DECOMP: Super simple sample demonstraiting use of LZSS decompression
*         routine.
* Copyright (C) 2004, 2006, 2007 by
* Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the lzss library.
*
* The lzss library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation; either version 3 of the
* License, or (at your option) any later version.
*
* The lzss library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : main
*   Description: This is the main function for this program it calls the
*                LZSS encoding routine using the command line arguments as
*                the file to decode and the decoded target.
*   Parameters : argc - number of parameters
*                argv - parameter list
*   Effects    : Encodes specified file
*   Returned   : status from DecodeLZSS
****************************************************************************/
int main(int argc, char *argv[])
{
    return EncodeLZSS("software/benchmarks/data/input_very_very_small.asc", "software/benchmarks/data/encoded");
}
