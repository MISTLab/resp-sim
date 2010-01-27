#include <stdio.h>
#include <stdlib.h>

/*
 * Listing 1 -- coder.h
 *
 * This header file contains the constants, declarations, and
 * prototypes needed to use the arithmetic coding routines.  These
 * declarations are for routines that need to interface with the
 * arithmetic coding stuff in coder.c
 *
 */

#define MAXIMUM_SCALE   16383  /* Maximum allowed frequency count */
#define ESCAPE          256    /* The escape symbol               */
#define DONE            -1     /* The output stream empty  symbol */
#define FLUSH           -2     /* The symbol to flush the model   */

/*
 * A symbol can either be represented as an int, or as a pair of
 * counts on a scale.  This structure gives a standard way of
 * defining it as a pair of counts.
 */
typedef struct {
                unsigned short int low_count;
                unsigned short int high_count;
                unsigned short int scale;
               } SYMBOL;

extern long underflow_bits;    /* The present underflow count in  */
                               /* the arithmetic coder.           */
/*
 * Function prototypes.
 */
void initialize_arithmetic_decoder( FILE *stream );
void remove_symbol_from_stream( FILE *stream, SYMBOL *s );
void initialize_arithmetic_encoder( void );
void encode_symbol( FILE *stream, SYMBOL *s );
void flush_arithmetic_encoder( FILE *stream );
short int get_current_count( SYMBOL *s );

/*
 * Listing 3 -- bitio.h
 *
 * This header file contains the function prototypes needed to use
 * the bitstream i/o routines.
 *
 */
short int input_bit( FILE *stream );
void initialize_output_bitstream( void );
void output_bit( FILE *stream, int bit );
void flush_output_bitstream( FILE *stream );
void initialize_input_bitstream( void );
long bit_ftell_output( FILE *stream );
long bit_ftell_input( FILE *stream );

/*
 * Listing 2 -- coder.c
 *
 * This file contains the code needed to accomplish arithmetic
 * coding of a symbol.  All the routines in this module need
 * to know in order to accomplish coding is what the probabilities
 * and scales of the symbol counts are.  This information is
 * generally passed in a SYMBOL structure.
 *
 * This code was first published by Ian H. Witten, Radford M. Neal,
 * and John G. Cleary in "Communications of the ACM" in June 1987,
 * and has been modified slightly for this article.  The code
 * is  published here with permission.
 */

#include <stdio.h>


/*
 * These four variables define the current state of the arithmetic
 * coder/decoder.  They are assumed to be 16 bits long.  Note that
 * by declaring them as short ints, they will actually be 16 bits
 * on most 80X86 and 680X0 machines, as well as VAXen.
 */
static unsigned short int code;  /* The present input code value       */
static unsigned short int low;   /* Start of the current code range    */
static unsigned short int high;  /* End of the current code range      */
long underflow_bits;             /* Number of underflow bits pending   */

/*
 * This routine must be called to initialize the encoding process.
 * The high register is initialized to all 1s, and it is assumed that
 * it has an infinite string of 1s to be shifted into the lower bit
 * positions when needed.
 */
void initialize_arithmetic_encoder()
{
    low = 0;
    high = 0xffff;
    underflow_bits = 0;
}

/*
 * This routine is called to encode a symbol.  The symbol is passed
 * in the SYMBOL structure as a low count, a high count, and a range,
 * instead of the more conventional probability ranges.  The encoding
 * process takes two steps.  First, the values of high and low are
 * updated to take into account the range restriction created by the
 * new symbol.  Then, as many bits as possible are shifted out to
 * the output stream.  Finally, high and low are stable again and
 * the routine returns.
 */
void encode_symbol( FILE *stream, SYMBOL *s )
{
    long range;
/*
 * These three lines rescale high and low for the new symbol.
 */
    range = (long) ( high-low ) + 1;
    high = low + (unsigned short int )
                 (( range * s->high_count ) / s->scale - 1 );
    low = low + (unsigned short int )
                 (( range * s->low_count ) / s->scale );
/*
 * This loop turns out new bits until high and low are far enough
 * apart to have stabilized.
 */
    for ( ; ; )
    {
/*
 * If this test passes, it means that the MSDigits match, and can
 * be sent to the output stream.
 */
        if ( ( high & 0x8000 ) == ( low & 0x8000 ) )
        {
            output_bit( stream, high & 0x8000 );
            while ( underflow_bits > 0 )
            {
                output_bit( stream, ~high & 0x8000 );
                underflow_bits--;
            }
        }
/*
 * If this test passes, the numbers are in danger of underflow, because
 * the MSDigits don't match, and the 2nd digits are just one apart.
 */
        else if ( ( low & 0x4000 ) && !( high & 0x4000 ))
        {
            underflow_bits += 1;
            low &= 0x3fff;
            high |= 0x4000;
        }
        else
            return ;
        low <<= 1;
        high <<= 1;
        high |= 1;
    }
}

/*
 * At the end of the encoding process, there are still significant
 * bits left in the high and low registers.  We output two bits,
 * plus as many underflow bits as are necessary.
 */
void flush_arithmetic_encoder( FILE *stream )
{
    output_bit( stream, low & 0x4000 );
    underflow_bits++;
    while ( underflow_bits-- > 0 )
        output_bit( stream, ~low & 0x4000 );
}

/*
 * When decoding, this routine is called to figure out which symbol
 * is presently waiting to be decoded.  This routine expects to get
 * the current model scale in the s->scale parameter, and it returns
 * a count that corresponds to the present floating point code:
 *
 *  code = count / s->scale
 */
short int get_current_count( SYMBOL *s )
{
    long range;
    short int count;

    range = (long) ( high - low ) + 1;
    count = (short int)
            ((((long) ( code - low ) + 1 ) * s->scale-1 ) / range );
    return( count );
}

/*
 * This routine is called to initialize the state of the arithmetic
 * decoder.  This involves initializing the high and low registers
 * to their conventional starting values, plus reading the first
 * 16 bits from the input stream into the code value.
 */
void initialize_arithmetic_decoder( FILE *stream )
{
    int i;

    code = 0;
    for ( i = 0 ; i < 16 ; i++ )
    {
        code <<= 1;
        code += input_bit( stream );
    }
    low = 0;
    high = 0xffff;
}

/*
 * Just figuring out what the present symbol is doesn't remove
 * it from the input bit stream.  After the character has been
 * decoded, this routine has to be called to remove it from the
 * input stream.
 */
void remove_symbol_from_stream( FILE *stream, SYMBOL *s )
{
    long range;

/*
 * First, the range is expanded to account for the symbol removal.
 */
    range = (long)( high - low ) + 1;
    high = low + (unsigned short int)
                 (( range * s->high_count ) / s->scale - 1 );
    low = low + (unsigned short int)
                 (( range * s->low_count ) / s->scale );
/*
 * Next, any possible bits are shipped out.
 */
    for ( ; ; )
    {
/*
 * If the MSDigits match, the bits will be shifted out.
 */
        if ( ( high & 0x8000 ) == ( low & 0x8000 ) )
        {
        }
/*
 * Else, if underflow is threatining, shift out the 2nd MSDigit.
 */
        else if ((low & 0x4000) == 0x4000  && (high & 0x4000) == 0 )
        {
            code ^= 0x4000;
            low   &= 0x3fff;
            high  |= 0x4000;
        }
/*
 * Otherwise, nothing can be shifted out, so I return.
 */
        else
            return;
        low <<= 1;
        high <<= 1;
        high |= 1;
        code <<= 1;
        code += input_bit( stream );
    }
}


/*
 * Listing 4 -- bitio.c
 *
 * This routine contains a set of bit oriented i/o routines
 * used for arithmetic data compression.  The important fact to
 * know about these is that the first bit is stored in the msb of
 * the first byte of the output, like you might expect.
 *
 * Both input and output maintain a local buffer so that they only
 * have to do block reads and writes.  This is done in spite of the
 * fact that C standard I/O does the same thing.  If these
 * routines are ever ported to assembly language the buffering
 * will come in handy.
 *
 */
#include <stdio.h>
#include <stdlib.h>


#define BUFFER_SIZE 256
static char buffer[ BUFFER_SIZE + 2 ]; /* This is the i/o buffer    */
static char *current_byte;             /* Pointer to current byte   */

static int output_mask;                /* During output, this byte  */
                                       /* contains the mask that is */
                                       /* applied to the output byte*/
                                       /* if the output bit is a 1  */

static int input_bytes_left;           /* During input, these three */
static int input_bits_left;            /* variables keep track of my*/
static int past_eof;                   /* input state.  The past_eof*/
                                       /* byte comes about because  */
                                       /* of the fact that there is */
                                       /* a possibility the decoder */
                                       /* can legitimately ask for  */
                                       /* more bits even after the  */
                                       /* entire file has been      */
                                       /* sucked dry.               */


/*
 * This routine is called once to initialze the output bitstream.
 * All it has to do is set up the current_byte pointer, clear out
 * all the bits in my current output byte, and set the output mask
 * so it will set the proper bit next time a bit is output.
 */
void initialize_output_bitstream()
{
    current_byte = buffer;
    *current_byte = 0;
    output_mask = 0x80;
}

/*
 * The output bit routine just has to set a bit in the current byte
 * if requested to.  After that, it updates the mask.  If the mask
 * shows that the current byte is filled up, it is time to go to the
 * next character in the buffer.  If the next character is past the
 * end of the buffer, it is time to flush the buffer.
 */
void output_bit( FILE *stream, int bit )
{
    if ( bit )
        *current_byte |= output_mask;
    output_mask >>= 1;
    if ( output_mask == 0 )
    {
        output_mask = 0x80;
        current_byte++;
        if ( current_byte == ( buffer + BUFFER_SIZE ) )
        {
            fwrite( buffer, 1, BUFFER_SIZE, stream );
            current_byte = buffer;
        }
        *current_byte = 0;
    }
}

/*
 * When the encoding is done, there will still be a lot of bits and
 * bytes sitting in the buffer waiting to be sent out.  This routine
 * is called to clean things up at that point.
 */
void flush_output_bitstream( FILE *stream )
{
    fwrite( buffer, 1, (size_t)( current_byte - buffer ) + 1, stream );
    current_byte = buffer;
}

/*
 * Bit oriented input is set up so that the next time the input_bit
 * routine is called, it will trigger the read of a new block.  That
 * is why input_bits_left is set to 0.
 */
void initialize_input_bitstream()
{
    input_bits_left = 0;
    input_bytes_left = 1;
    past_eof = 0;
}

/*
 * This routine reads bits in from a file.  The bits are all sitting
 * in a buffer, and this code pulls them out, one at a time.  When the
 * buffer has been emptied, that triggers a new file read, and the
 * pointers are reset.  This routine is set up to allow for two dummy
 * bytes to be read in after the end of file is reached.  This is because
 * we have to keep feeding bits into the pipeline to be decoded so that
 * the old stuff that is 16 bits upstream can be pushed out.
 */
short int input_bit( FILE *stream )
{
    if ( input_bits_left == 0 )
    {
        current_byte++;
        input_bytes_left--;
        input_bits_left = 8;
        if ( input_bytes_left == 0 )
        {
            input_bytes_left = fread( buffer, 1, BUFFER_SIZE, stream );
            if ( input_bytes_left == 0 )
            {
                if ( past_eof )
                {
                    fprintf( stderr, "Bad input file\n" );
                    exit( -1 );
                }
                else
                {
                    past_eof = 1;
                    input_bytes_left = 2;
                }
            }
            current_byte = buffer;
        }
    }
    input_bits_left--;
    return ( ( *current_byte >> input_bits_left ) & 1 );
}

/*
 * When monitoring compression ratios, we need to know how many
 * bytes have been output so far.  This routine takes care of telling
 * how many bytes have been output, including pending bytes that
 * haven't actually been written out.
 */
long bit_ftell_output( FILE *stream )
{
    long total;

    total = ftell( stream );
    total += current_byte - buffer;
    total += underflow_bits/8;
    return( total );
}

/*
 * When monitoring compression ratios, we need to know how many bits
 * have been read in so far.  This routine tells how many bytes have
 * been read in, excluding bytes that are pending in the buffer.
 */
long bit_ftell_input( FILE *stream )
{
    return( ftell( stream ) - input_bytes_left + 1 );
}

/*
 * Listing 5 -- bill.c
 *
 * This short demonstration program will use arithmetic data
 * compression to encode and then decode a string that only uses
 * the letters out of the phrase "BILL GATES".  It uses a fixed
 * table of probabilities that is hardcoded in.  Note that it
 * differs from the example in the article in that it adds a single
 * new symbol, '\0', which is used to indicate the end of the string.
 *
 * To build this program:
 *
 * Turbo C:     tcc -w bill.c bitio.c coder.c
 * QuickC:      qcl /W3 bill.c bitio.c coder.c
 * Zortech:     ztc bill.c bitio.c coder.c
 * *NIX:        cc -o bill bill.c bitio.c coder.c
 *
 */

#include <stdio.h>
#include <stdlib.h>


/*
 * Declarations for local routines.
 */
void compress( void );
void expand( void );
void convert_int_to_symbol( char c, SYMBOL *s );
char convert_symbol_to_int( unsigned int count, SYMBOL *s );
void error_exit( char *message );

/*
 * This is a the probability table for the symbol set used
 * in this example.  Each symbols has a low and high range,
 * and the total count is fixed at 11.
 */
struct {
          char c;
          unsigned short int low;
          unsigned short int high;
       } probabilities[]= {{ 'B',  0,  1  },
                           { 'I',  1,  2  },
                           { 'L',  2,  4  },
                           { ' ',  4,  5  },
                           { 'G',  5,  6  },
                           { 'A',  6,  7  },
                           { 'T',  7,  8  },
                           { 'E',  8,  9  },
                           { 'S',  9,  10 },
                           { '\0', 10, 11  }
                          };
/*
 * This example program compresses an input string, sending
 * the output to a file.  It then expands the output file,
 * sending the decoded characters to the screen.
 */
int main()
{
//    compress();
    expand();
    return 0;
}

/*
 * This is the compress routine.  It shows the basic algorithm for
 * the compression programs used in this article.  First, an input
 * characters is loaded.  The modeling routines are called to
 * convert the character to a symbol, which has a high, low and
 * range.  Finally, the arithmetic coder module is called to
 * output the symbols to the bit stream.
 */
void compress()
{
    int i;
    char c;
    SYMBOL s;
    FILE *compressed_file;
    static char *input = "GLIB BATES";

    compressed_file=fopen( "software/benchmarks/data/test.cmp", "wb" );
    if ( compressed_file == NULL )
        error_exit( "Could not open output file" );
    puts( "Compressing..." );
    initialize_output_bitstream();
    initialize_arithmetic_encoder();
    for ( i=0 ; ; )
    {
        c = input[ i++ ];
        convert_int_to_symbol( c, &s );
        encode_symbol( compressed_file, &s );
        if ( c == '\0' )
            break;
    }
    flush_arithmetic_encoder( compressed_file );
    flush_output_bitstream( compressed_file );
    fclose( compressed_file);
}

/*
 * This expansion routine demonstrates the basic algorithm used for
 * decompression in this article.  It first goes to the modeling
 * module and gets the scale for the current context.  (Note that
 * the scale is fixed here, since this is not an adaptive model).
 * It then asks the arithmetic decoder to give a high and low
 * value for the current input number scaled to match the current
 * range.  Finally, it asks the modeling unit to convert the
 * high and low values to a symbol.
 */
void expand()
{
    FILE *compressed_file;
    SYMBOL s;
    char c;
    int count;

    compressed_file=fopen( "software/benchmarks/data/test_comp.cmp", "rb" );
    if ( compressed_file == NULL )
        error_exit( "Could not open output file" );
    puts( "Decoding..." );
    printf( "Incoming characters: " );
    initialize_input_bitstream();
    initialize_arithmetic_decoder( compressed_file );
    for ( ; ; )
    {
        s.scale = 11;
        count = get_current_count( &s );
        c = convert_symbol_to_int( count, &s );
        if ( c == '\0' )
            break;
        remove_symbol_from_stream( compressed_file, &s );
        putc( c, stdout );
    }
    putc( '\n', stdout );
}

/*
 * This routine is called to convert a character read in from
 * the text input stream to a low, high, range SYMBOL.  This is
 * part of the modeling function.  In this case, all that needs
 * to be done is to find the character in the probabilities table
 * and then retrieve the low and high values for that symbol.
 */
void convert_int_to_symbol( char c, SYMBOL *s )
{
    int i;

    i=0;
    for ( ; ; )
    {
        if ( c == probabilities[ i ].c )
        {
            s->low_count = probabilities[ i ].low;
            s->high_count = probabilities[ i ].high;
            s->scale = 11;
            return;
        }
        if ( probabilities[i].c == '\0' )
            error_exit( "Trying to encode a char not in the table" );
        i++;
    }
}

/*
 * This modeling function is called to convert a SYMBOL value
 * consisting of a low, high, and range value into a text character
 * that can be sent to a file.  It does this by finding the symbol
 * in the probability table that straddles the current range.
 */
char convert_symbol_to_int( unsigned int count, SYMBOL *s )
{
    int i;

    i = 0;
    for ( ; ; )
    {
        if ( count >= probabilities[ i ].low &&
             count < probabilities[ i ].high )
        {
            s->low_count = probabilities[ i ].low;
            s->high_count = probabilities[ i ].high;
            s->scale = 11;
            return( probabilities[ i ].c );
        }
        if ( probabilities[ i ].c == '\0' )
            error_exit( "Failure to decode character" );
        i++;
    }
}

/*
 * A generic error routine.
 */
void error_exit( char *message )
{
    puts( message );
    exit( -1 );
}

