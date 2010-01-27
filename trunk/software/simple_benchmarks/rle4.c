/***************************************************************************
*            Header for Run Length Encoding and Decoding Library
*
*   File    : rle.h
*   Purpose : Provides prototypes for functions that use run length coding
*             to encode/decode files.
*   Author  : Michael Dipperstein
*   Date    : April 30, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: rle.h,v 1.3 2007/09/08 17:10:24 michael Exp $
*   $Log: rle.h,v $
*   Revision 1.3  2007/09/08 17:10:24  michael
*   Changes required for LGPL v3.
*
*   Revision 1.2  2006/09/10 05:08:36  michael
*   Add prototypes for variant packbits algorithm.
*
*   Revision 1.1.1.1  2004/05/03 03:56:49  michael
*   Initial version
*
*
****************************************************************************
*
* RLE: An ANSI C Run Length Encoding/Decoding Routines
* Copyright (C) 2004, 2006-2007 by
*       Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the RLE library.
*
* The RLE library is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation; either version 3 of the License, or (at
* your option) any later version.
*
* The RLE library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

#ifndef _RLE_H_
#define _RLE_H_

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/* traditional RLE encodeing/decoding */
int RleEncodeFile(const char *inFile, const char *outFile);
int RleDecodeFile(const char *inFile, const char *outFile);

/* variant of packbits RLE encodeing/decoding */
int VPackBitsEncodeFile(const char *inFile, const char *outFile);
int VPackBitsDecodeFile(const char *inFile, const char *outFile);

#endif  /* ndef _RLE_H_ */
/***************************************************************************
*                       Command Line Option Parser
*
*   File    : optlist.h
*   Purpose : Header for getopt style command line option parsing
*   Author  : Michael Dipperstein
*   Date    : August 1, 2007
*
****************************************************************************
*   HISTORY
*
*   $Id: optlist.h,v 1.1.1.2 2007/09/04 04:45:42 michael Exp $
*   $Log: optlist.h,v $
*   Revision 1.1.1.2  2007/09/04 04:45:42  michael
*   Added FreeOptList.
*
*   Revision 1.1.1.1  2007/08/07 05:01:48  michael
*   Initial Release
*
****************************************************************************
*
* OptList: A command line option parsing library
* Copyright (C) 2007 by Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the OptList library.
*
* OptList is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or (at
* your option) any later version.
*
* OptList is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/
#ifndef OPTLIST_H
#define OPTLIST_H

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/

/***************************************************************************
*                                 MACROS
***************************************************************************/

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#define    OL_NOINDEX    -1        /* this option has no arguement */

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/
typedef struct option_t
{
    char option;
    char *argument;
    int argIndex;
    struct option_t *next;
} option_t;

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
option_t *GetOptList(int argc, char *const argv[], char *const options);
void FreeOptList(option_t *list);

#endif  /* ndef OPTLIST_H */
/***************************************************************************
*                       Command Line Option Parser
*
*   File    : optlist.c
*   Purpose : Provide getopt style command line option parsing
*   Author  : Michael Dipperstein
*   Date    : August 1, 2007
*
****************************************************************************
*   HISTORY
*
*   $Id: optlist.c,v 1.1.1.2 2007/09/04 04:45:42 michael Exp $
*   $Log: optlist.c,v $
*   Revision 1.1.1.2  2007/09/04 04:45:42  michael
*   Added FreeOptList.
*
*   Revision 1.1.1.1  2007/08/07 05:01:48  michael
*   Initial Release
*
****************************************************************************
*
* OptList: A command line option parsing library
* Copyright (C) 2007 by Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the OptList library.
*
* OptList is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or (at
* your option) any later version.
*
* OptList is distributed in the hope that it will be useful, but
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

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
option_t *MakeOpt(const char option, char *const argument, const int index);

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : GetOptList
*   Description: This function is similar to the POSIX function getopt.  All
*                options and their corresponding arguments are returned in a
*                linked list.  This function should only be called once per
*                an option list and it does not modify argv or argc.
*   Parameters : argc - the number of command line arguments (including the
*                       name of the executable)
*                argv - pointer to the open binary file to write encoded
*                       output
*                options - getopt style option list.  A NULL terminated
*                          string of single character options.  Follow an
*                          option with a colon to indicate that it requires
*                          an argument.
*   Effects    : Creates a link list of command line options and their
*                arguments.
*   Returned   : option_t type value where the option and arguement fields
*                contain the next option symbol and its argument (if any).
*                The argument field will be set to NULL if the option is
*                specified as having no arguments or no arguments are found.
*                The option field will be set to PO_NO_OPT if no more
*                options are found.
*
*   NOTE: The caller is responsible for freeing up the option list when it
*         is no longer needed.
****************************************************************************/
option_t *GetOptList(const int argc, char *const argv[], char *const options)
{
    int nextArg;
    option_t *head, *tail;
    int optIndex;

    /* start with first argument and nothing found */
    nextArg = 1;
    head = NULL;
    tail = NULL;

    /* loop through all of the command line arguments */
    while (nextArg < argc)
    {
        if ((strlen(argv[nextArg]) > 1) && ('-' == argv[nextArg][0]))
        {
            /* possible option */
            optIndex = 0;

            /* attempt to find a matching option */
            while ((options[optIndex] != '\0') &&
                (options[optIndex] != argv[nextArg][1]))
            {
                do
                {
                    optIndex++;
                }
                while ((options[optIndex] != '\0') &&
                    (':' == options[optIndex]));
            }

            if (options[optIndex] == argv[nextArg][1])
            {
                /* we found the matching option */
                if (NULL == head)
                {
                    head = MakeOpt(options[optIndex], NULL, OL_NOINDEX);
                    tail = head;
                }
                else
                {
                    tail->next = MakeOpt(options[optIndex], NULL, OL_NOINDEX);
                    tail = tail->next;
                }

                if (':' == options[optIndex + 1])
                {
                    /* the option found should have a text arguement */
                    if (strlen(argv[nextArg]) > 2)
                    {
                        /* no space between argument and option */
                        tail->argument = &(argv[nextArg][2]);
                        tail->argIndex = nextArg;
                    }
                    else if (nextArg < argc)
                    {
                        /* there must be space between the argument option */
                        nextArg++;
                        tail->argument = argv[nextArg];
                        tail->argIndex = nextArg;
                    }
                }
            }
        }

        nextArg++;
    }

    return head;
}

/****************************************************************************
*   Function   : MakeOpt
*   Description: This function uses malloc to allocate space for an option_t
*                type structure and initailizes the structure with the
*                values passed as a parameter.
*   Parameters : option - this option character
*                argument - pointer string containg the argument for option.
*                           Use NULL for no argument
*                index - argv[index] contains argument us OL_NOINDEX for
*                        no argument
*   Effects    : A new option_t type variable is created on the heap.
*   Returned   : Pointer to newly created and initialized option_t type
*                structure.  NULL if space for structure can't be allocated.
****************************************************************************/
option_t *MakeOpt(const char option, char *const argument, const int index)
{
    option_t *opt;

    opt = malloc(sizeof(option_t));

    if (opt != NULL)
    {
        opt->option = option;
        opt->argument = argument;
        opt->argIndex = index;
        opt->next = NULL;
    }
    else
    {
        perror("Failed to Allocate option_t");
    }

    return opt;
}

/****************************************************************************
*   Function   : FreeOptList
*   Description: This function will free all the elements in an option_t
*                type linked list starting from the node passed as a
*                parameter.
*   Parameters : list - head of linked list to be freed
*   Effects    : All elements of the linked list pointed to by list will
*                be freed and list will be set to NULL.
*   Returned   : None
****************************************************************************/
void FreeOptList(option_t *list)
{
    option_t *head, *next;

    head = list;
    list = NULL;

    while (head != NULL)
    {
        next = head->next;
        free(head);
        head = next;
    }

    return;
}
/***************************************************************************
*                 Run Length Encoding and Decoding Library
*
*   File    : rle.c
*   Purpose : Use run length coding to compress/decompress files.  The
*             version of encoding used by this library only provides a run
*             length if the last two symbols are matching.  This method
*             avoids the need to include run lengths for runs of only 1
*             symbol.  It also avoids the need for escape characters.
*   Author  : Michael Dipperstein
*   Date    : April 30, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: rle.c,v 1.2 2007/09/08 17:10:24 michael Exp $
*   $Log: rle.c,v $
*   Revision 1.2  2007/09/08 17:10:24  michael
*   Changes required for LGPL v3.
*
*   Revision 1.1.1.1  2004/05/03 03:56:49  michael
*   Initial version
*
****************************************************************************
*
* RLE: An ANSI C Run Length Encoding/Decoding Routines
* Copyright (C) 2004, 2007 by
*       Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the RLE library.
*
* The RLE library is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation; either version 3 of the License, or (at
* your option) any later version.
*
* The RLE library is distributed in the hope that it will be useful, but
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
#include <limits.h>

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#define FALSE       0
#define TRUE        1

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/***************************************************************************
*   Function   : RleEncodeFile
*   Description: This routine reads an input file and writes out a run
*                length encoded version of that file.
*   Parameters : inFile - Name of file to encode
*                outFile - Name of file to write encoded output to
*   Effects    : File is encoded using RLE
*   Returned   : TRUE for success, otherwise FALSE.
***************************************************************************/
int RleEncodeFile(const char *inFile, const char *outFile)
{
    FILE *fpIn;                         /* uncoded input */
    FILE *fpOut;                        /* encoded output */
    int currChar, prevChar;             /* current and previous characters */
    unsigned char count;                /* number of characters in a run */

    /* open input and output files */
    if ((fpIn = fopen(inFile, "rb")) == NULL)
    {
        perror(inFile);
        return FALSE;
    }

    if (outFile == NULL)
    {
        fpOut = stdout;     /* default to stdout */
    }
    else
    {
        if ((fpOut = fopen(outFile, "wb")) == NULL)
        {
            fclose(fpIn);
            perror(outFile);
            return FALSE;
        }
    }

    /* encode inFile */
    prevChar = EOF;     /* force next char to be different */
    count = 0;

    /* read input until there's nothing left */
    while ((currChar = fgetc(fpIn)) != EOF)
    {
        fputc(currChar, fpOut);

        /* check for run */
        if (currChar == prevChar)
        {
            /* we have a run.  count run length */
            count = 0;

            while ((currChar = fgetc(fpIn)) != EOF)
            {
                if (currChar == prevChar)
                {
                    count++;

                    if (count == UCHAR_MAX)
                    {
                        /* count is as long as it can get */
                        fputc(count, fpOut);

                        /* force next char to be different */
                        prevChar = EOF;
                        break;
                    }
                }
                else
                {
                    /* run ended */
                    fputc(count, fpOut);
                    fputc(currChar, fpOut);
                    prevChar = currChar;
                    break;
                }
            }
        }
        else
        {
            /* no run */
            prevChar = currChar;
        }

        if (currChar == EOF)
        {
            /* run ended because of EOF */
            fputc(count, fpOut);
            break;
        }
    }

    /* close all open files */
    fclose(fpOut);
    fclose(fpIn);

    return TRUE;
}

/***************************************************************************
*   Function   : RleDecodeFile
*   Description: This routine opens a run length encoded file, and decodes
*                it to an output file.
*   Parameters : inFile - Name of file to decode
*                outFile - Name of file to write decoded output to
*   Effects    : Encoded file is decoded
*   Returned   : TRUE for success, otherwise FALSE.
***************************************************************************/
int RleDecodeFile(const char *inFile, const char *outFile)
{
    FILE *fpIn;                         /* encoded input */
    FILE *fpOut;                        /* uncoded output */
    int currChar, prevChar;             /* current and previous characters */
    unsigned char count;                /* number of characters in a run */

    /* open input and output files */
    if ((fpIn = fopen(inFile, "rb")) == NULL)
    {
        perror(inFile);
        return FALSE;
    }

    if (outFile == NULL)
    {
        fpOut = stdout;     /* default to stdout */
    }
    else
    {
        if ((fpOut = fopen(outFile, "wb")) == NULL)
        {
            fclose(fpIn);
            perror(outFile);
            return FALSE;
        }
    }

    /* decode inFile */
    prevChar = EOF;     /* force next char to be different */

    /* read input until there's nothing left */
    while ((currChar = fgetc(fpIn)) != EOF)
    {
        fputc(currChar, fpOut);

        /* check for run */
        if (currChar == prevChar)
        {
            /* we have a run.  write it out. */
            count = fgetc(fpIn);
            while (count > 0)
            {
                fputc(currChar, fpOut);
                count--;
            }

            prevChar = EOF;     /* force next char to be different */
        }
        else
        {
            /* no run */
            prevChar = currChar;
        }
    }

    /* close all open files */
    fclose(fpOut);
    fclose(fpIn);

    return TRUE;
}
/***************************************************************************
*               Variant Packbits Encoding and Decoding Library
*
*   File    : vpackbits.c
*   Purpose : Use a variant of packbits run length coding to compress and
*             decompress files.  This packbits variant begins each block of
*             data with the a byte header that is decoded as follows.
*
*             Byte (n)   | Meaning
*             -----------+-------------------------------------
*             0 - 127    | Copy the next n + 1 bytes
*             -128 - -1  | Make -n + 2 copies of the next byte
*
*   Author  : Michael Dipperstein
*   Date    : September 7, 2006
*
****************************************************************************
*   UPDATES
*
*   $Id: vpackbits.c,v 1.4 2007/09/08 17:12:27 michael Exp $
*   $Log: vpackbits.c,v $
*   Revision 1.4  2007/09/08 17:12:27  michael
*   Corrected error decoding maximum length runs.
*   Replace getopt with optlist.
*   Changes required for LGPL v3.
*
*   Revision 1.3  2007/02/13 05:29:43  michael
*   trimmed spaces.
*
*   Revision 1.2  2006/09/20 13:13:30  michael
*   Minor modifications to look more like description on my web page.
*
*   Revision 1.1  2006/09/10 05:12:57  michael
*   Initial release
*
****************************************************************************
*
* VPackBits: ANSI C PackBits Style Run Length Encoding/Decoding Routines
* Copyright (C) 2006-2007 by
*       Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the RLE library.
*
* The RLE library is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation; either version 3 of the License, or (at
* your option) any later version.
*
* The RLE library is distributed in the hope that it will be useful, but
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
#include <limits.h>

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#define FALSE       0
#define TRUE        1

#define MIN_RUN     3                   /* minimum run length to encode */
#define MAX_RUN     (128 + MIN_RUN - 1) /* maximum run length to encode */
#define MAX_COPY    128                 /* maximum characters to copy */

/* maximum that can be read before copy block is written */
#define MAX_READ    (MAX_COPY + MIN_RUN - 1)

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/***************************************************************************
*   Function   : VPackBitsEncodeFile
*   Description: This routine reads an input file and writes out a run
*                length encoded version of that file.  The technique used
*                is a variation of the packbits technique.
*   Parameters : inFile - Name of file to encode
*                outFile - Name of file to write encoded output to
*   Effects    : File is encoded using RLE
*   Returned   : TRUE for success, otherwise FALSE.
***************************************************************************/
int VPackBitsEncodeFile(const char *inFile, const char *outFile)
{
    FILE *fpIn;                         /* uncoded input */
    FILE *fpOut;                        /* encoded output */
    int currChar;                       /* current character */
    unsigned char charBuf[MAX_READ];    /* buffer of already read characters */
    unsigned char count;                /* number of characters in a run */

    /* open input and output files */
    if ((fpIn = fopen(inFile, "rb")) == NULL)
    {
        perror(inFile);
        return FALSE;
    }

    if (outFile == NULL)
    {
        fpOut = stdout;     /* default to stdout */
    }
    else
    {
        if ((fpOut = fopen(outFile, "wb")) == NULL)
        {
            fclose(fpIn);
            perror(outFile);
            return FALSE;
        }
    }

    /* prime the read loop */
    currChar = fgetc(fpIn);
    count = 0;

    /* read input until there's nothing left */
    while (currChar != EOF)
    {
        charBuf[count] = (unsigned char)currChar;

        count++;

        if (count >= MIN_RUN)
        {
            int i;

            /* check for run  charBuf[count - 1] .. charBuf[count - MIN_RUN]*/
            for (i = 2; i <= MIN_RUN; i++)
            {
                if (currChar != charBuf[count - i])
                {
                    /* no run */
                    i = 0;
                    break;
                }
            }

            if (i != 0)
            {
                /* we have a run write out buffer before run*/
                int nextChar;

                if (count > MIN_RUN)
                {
                    /* block size - 1 followed by contents */
                    fputc(count - MIN_RUN - 1, fpOut);
                    fwrite(charBuf, sizeof(unsigned char), count - MIN_RUN,
                        fpOut);
                }


                /* determine run length (MIN_RUN so far) */
                count = MIN_RUN;

                while ((nextChar = fgetc(fpIn)) == currChar)
                {
                    count++;
                    if (MAX_RUN == count)
                    {
                        /* run is at max length */
                        break;
                    }
                }

                /* write out encoded run length and run symbol */
                fputc((char)((int)(MIN_RUN - 1) - (int)(count)), fpOut);
                fputc(currChar, fpOut);

                if ((nextChar != EOF) && (count != MAX_RUN))
                {
                    /* make run breaker start of next buffer */
                    charBuf[0] = nextChar;
                    count = 1;
                }
                else
                {
                    /* file or max run ends in a run */
                    count = 0;
                }
            }
        }

        if (MAX_READ == count)
        {
            int i;

            /* write out buffer */
            fputc(MAX_COPY - 1, fpOut);
            fwrite(charBuf, sizeof(unsigned char), MAX_COPY, fpOut);

            /* start a new buffer */
            count = MAX_READ - MAX_COPY;

            /* copy excess to front of buffer */
            for (i = 0; i < count; i++)
            {
                charBuf[i] = charBuf[MAX_COPY + i];
            }
        }

        currChar = fgetc(fpIn);
    }

    /* write out last buffer */
    if (0 != count)
    {
        if (count <= MAX_COPY)
        {
            /* write out entire copy buffer */
            fputc(count - 1, fpOut);
            fwrite(charBuf, sizeof(unsigned char), count, fpOut);
        }
        else
        {
            /* we read more than the maximum for a single copy buffer */
            fputc(MAX_COPY - 1, fpOut);
            fwrite(charBuf, sizeof(unsigned char), MAX_COPY, fpOut);

            /* write out remainder */
            count -= MAX_COPY;
            fputc(count - 1, fpOut);
            fwrite(&charBuf[MAX_COPY], sizeof(unsigned char), count, fpOut);
        }
    }

    /* close all open files */
    fclose(fpOut);
    fclose(fpIn);

    return TRUE;
}

/***************************************************************************
*   Function   : VPackBitsDecodeFile
*   Description: This routine opens a file encoded by a variant of the
*                packbits run length encoding, and decodes it to an output
*                file.
*   Parameters : inFile - Name of file to decode
*                outFile - Name of file to write decoded output to
*   Effects    : Encoded file is decoded
*   Returned   : TRUE for success, otherwise FALSE.
***************************************************************************/
int VPackBitsDecodeFile(const char *inFile, const char *outFile)
{
    FILE *fpIn;                         /* encoded input */
    FILE *fpOut;                        /* uncoded output */
    int countChar;                      /* run/copy count */
    int currChar;                       /* current character */

    /* open input and output files */
    if ((fpIn = fopen(inFile, "rb")) == NULL)
    {
        perror(inFile);
        return FALSE;
    }

    if (outFile == NULL)
    {
        fpOut = stdout;     /* default to stdout */
    }
    else
    {
        if ((fpOut = fopen(outFile, "wb")) == NULL)
        {
            fclose(fpIn);
            perror(outFile);
            return FALSE;
        }
    }

    /* decode inFile */

    /* read input until there's nothing left */
    while ((countChar = fgetc(fpIn)) != EOF)
    {
        countChar = (char)countChar;    /* force sign extension */

        if (countChar < 0)
        {
            /* we have a run write out  2 - countChar copies */
            countChar = (MIN_RUN - 1) - countChar;

            if (EOF == (currChar = fgetc(fpIn)))
            {
                fprintf(stderr, "Run block is too short!\n");
                countChar = 0;
            }

            while (countChar > 0)
            {
                fputc(currChar, fpOut);
                countChar--;
            }
        }
        else
        {
            /* we have a block of countChar + 1 symbols to copy */
            for (countChar++; countChar > 0; countChar--)
            {
                if ((currChar = fgetc(fpIn)) != EOF)
                {
                    fputc(currChar, fpOut);
                }
                else
                {
                    fprintf(stderr, "Copy block is too short!\n");
                    break;
                }
            }
        }
    }

    /* close all open files */
    fclose(fpOut);
    fclose(fpIn);

    return TRUE;
}
/***************************************************************************
*              Sample Program Using Run Length Encoding Library
*
*   File    : sample.c
*   Purpose : Demonstrate usage of run length encoding library
*   Author  : Michael Dipperstein
*   Date    : April 30, 2004
*
****************************************************************************
*   UPDATES
*
*   $Id: sample.c,v 1.3 2007/09/08 17:11:44 michael Exp $
*   $Log: sample.c,v $
*   Revision 1.3  2007/09/08 17:11:44  michael
*   Replace getopt with optlist.
*   Changes required for LGPL v3.
*
*   Revision 1.2  2006/09/10 05:11:22  michael
*   Add sample usage for variant packbits functions.
*
*   Revision 1.1.1.1  2004/05/03 03:56:49  michael
*   Initial version
*
*
****************************************************************************
*
* SAMPLE: Sample usage of Run Length Encoding Library
* Copyright (C) 2004, 2006-2007 by
*       Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the RLE library.
*
* The RLE library is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation; either version 3 of the License, or (at
* your option) any later version.
*
* The RLE library is distributed in the hope that it will be useful, but
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
typedef enum
{
    ENCODE,
    DECODE
} MODES;

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
char *RemovePath(char *fullPath);

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : main
*   Description: This is the main function for this program, it validates
*                the command line input and, if valid, it will call
*                functions to encode or decode a file using a run length
*                encoding algorithm.
*   Parameters : argc - number of parameters
*                argv - parameter list
*   Effects    : Encodes/Decodes input file
*   Returned   : EXIT_SUCCESS for success, otherwise EXIT_FAILURE.
****************************************************************************/
int main(int argc, char *argv[])
{
    option_t *optList, *thisOpt;
    MODES mode;
    unsigned char usePackBits;

    /* initialize data */
    const char *inFile = "software/benchmarks/data/rle_out_3.out";
    const char *outFile = "software/benchmarks/data/rle_out_4.out";
    mode = DECODE;
    usePackBits = 1;

    /* we have valid parameters encode or decode */
    if (mode == ENCODE)
    {
        if (usePackBits)
        {
            VPackBitsEncodeFile(inFile, outFile);
        }
        else
        {
            RleEncodeFile(inFile, outFile);
        }
    }
    else
    {
        if (usePackBits)
        {
            VPackBitsDecodeFile(inFile, outFile);
        }
        else
        {
            RleDecodeFile(inFile, outFile);
        }
    }

    return EXIT_SUCCESS;
}

/****************************************************************************
*   Function   : RemovePath
*   Description: This is function accepts a pointer to the name of a file
*                along with path information and returns a pointer to the
*                character that is not part of the path.
*   Parameters : fullPath - pointer to an array of characters containing
*                           a file name and possible path modifiers.
*   Effects    : None
*   Returned   : Returns a pointer to the first character after any path
*                information.
****************************************************************************/
char *RemovePath(char *fullPath)
{
    int i;
    char *start, *tmp;                          /* start of file name */
    const char delim[3] = {'\\', '/', ':'};     /* path deliminators */

    start = fullPath;

    /* find the first character after all file path delimiters */
    for (i = 0; i < 3; i++)
    {
        tmp = strrchr(start, delim[i]);

        if (tmp != NULL)
        {
            start = tmp + 1;
        }
    }

    return start;
}
