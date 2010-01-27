#include <stdlib.h>
#include <stdio.h>

FILE *in;
FILE *out;

/* INTERFACE TO THE MODEL. */


/* THE SET OF SYMBOLS THAT MAY BE ENCODED. */

#define No_of_chars 256         /* Number of character symbols      */
#define EOF_symbol (No_of_chars+1)  /* Index of EOF symbol              */

#define No_of_symbols (No_of_chars+1)   /* Total number of symbols          */


/* TRANSLATION TABLES BETWEEN CHARACTERS AND SYMBOL INDEXES. */

int char_to_index[No_of_chars];     /* To index from character          */
unsigned char index_to_char[No_of_symbols+1]; /* To character from index    */


/* CUMULATIVE FREQUENCY TABLE. */

#define Max_frequency 16383     /* Maximum allowed frequency count  */

int cum_freq[No_of_symbols+1];      /* Cumulative symbol frequencies    */


/* DECLARATIONS USED FOR ARITHMETIC ENCODING AND DECODING */


/* SIZE OF ARITHMETIC CODE VALUES. */

#define Code_value_bits 16      /* Number of bits in a code value   */
typedef long code_value;        /* Type of an arithmetic code value */

#define Top_value (((long)1<<Code_value_bits)-1)      /* Largest code value */


/* HALF AND QUARTER POINTS IN THE CODE VALUE RANGE. */

#define First_qtr (Top_value/4+1)   /* Point after first quarter        */
#define Half      (2*First_qtr)     /* Point after first half           */
#define Third_qtr (3*First_qtr)     /* Point after third quarter        */

/* THE ADAPTIVE SOURCE MODEL */

int freq[No_of_symbols+1];  /* Symbol frequencies                       */


/* INITIALIZE THE MODEL. */

start_model()
{   int i;
    for (i = 0; i<No_of_chars; i++) {       /* Set up tables that       */
        char_to_index[i] = i+1;         /* translate between symbol */
        index_to_char[i+1] = i;         /* indexes and characters.  */
    }
    for (i = 0; i<=No_of_symbols; i++) {    /* Set up initial frequency */
        freq[i] = 1;                /* counts to be one for all */
        cum_freq[i] = No_of_symbols-i;      /* symbols.                 */
    }
    freq[0] = 0;                /* Freq[0] must not be the  */
}                       /* same as freq[1].         */


/* UPDATE THE MODEL TO ACCOUNT FOR A NEW SYMBOL. */

update_model(symbol)
    int symbol;         /* Index of new symbol                      */
{   int i;          /* New index for symbol                     */
    if (cum_freq[0]==Max_frequency) {       /* See if frequency counts  */
        int cum;                /* are at their maximum.    */
        cum = 0;
        for (i = No_of_symbols; i>=0; i--) {    /* If so, halve all the     */
            freq[i] = (freq[i]+1)/2;        /* counts (keeping them     */
            cum_freq[i] = cum;          /* non-zero).               */
            cum += freq[i];
        }
    }
    for (i = symbol; freq[i]==freq[i-1]; i--) ; /* Find symbol's new index. */
    if (i<symbol) {
        int ch_i, ch_symbol;
        ch_i = index_to_char[i];        /* Update the translation   */
        ch_symbol = index_to_char[symbol];  /* tables if the symbol has */
        index_to_char[i] = ch_symbol;           /* moved.                   */
        index_to_char[symbol] = ch_i;
        char_to_index[ch_i] = symbol;
        char_to_index[ch_symbol] = i;
    }
    freq[i] += 1;               /* Increment the frequency  */
    while (i>0) {               /* count for the symbol and */
        i -= 1;                 /* update the cumulative    */
        cum_freq[i] += 1;           /* frequencies.             */
    }
}

/* ARITHMETIC ENCODING ALGORITHM. */

static void bit_plus_follow();  /* Routine that follows                     */


/* CURRENT STATE OF THE ENCODING. */

static code_value low, high;    /* Ends of the current code region          */
static long bits_to_follow; /* Number of opposite bits to output after  */
                /* the next bit.                            */


/* START ENCODING A STREAM OF SYMBOLS. */

start_encoding()
{   low = 0;                    /* Full code range.         */
    high = Top_value;
    bits_to_follow = 0;             /* No bits to follow next.  */
}


/* ENCODE A SYMBOL. */

encode_symbol(symbol,cum_freq)
    int symbol;         /* Symbol to encode                         */
    int cum_freq[];     /* Cumulative symbol frequencies            */
{   long range;         /* Size of the current code region          */
    range = (long)(high-low)+1;
    high = low +                /* Narrow the code region   */
      (range*cum_freq[symbol-1])/cum_freq[0]-1; /* to that allotted to this */
    low = low +                 /* symbol.                  */
      (range*cum_freq[symbol])/cum_freq[0];
    for (;;) {                  /* Loop to output bits.     */
        if (high<Half) {
            bit_plus_follow(0);         /* Output 0 if in low half. */
        } 
        else if (low>=Half) {           /* Output 1 if in high half.*/
            bit_plus_follow(1);
            low -= Half;
            high -= Half;           /* Subtract offset to top.  */
        }
        else if (low>=First_qtr         /* Output an opposite bit   */
              && high<Third_qtr) {      /* later if in middle half. */
            bits_to_follow += 1;
            low -= First_qtr;           /* Subtract offset to middle*/
            high -= First_qtr;
        }
        else break;             /* Otherwise exit loop.     */
        low = 2*low;
        high = 2*high+1;            /* Scale up code range.     */
    }
}


/* FINISH ENCODING THE STREAM. */

done_encoding()
{   bits_to_follow += 1;            /* Output two bits that     */
    if (low<First_qtr) bit_plus_follow(0);  /* select the quarter that  */
    else bit_plus_follow(1);            /* the current code range   */
}                       /* contains.                */


/* OUTPUT BITS PLUS FOLLOWING OPPOSITE BITS. */

static void bit_plus_follow(bit)
    int bit;
{   output_bit(bit);                /* Output the bit.          */
    while (bits_to_follow>0) {
        output_bit(!bit);           /* Output bits_to_follow    */
        bits_to_follow -= 1;            /* opposite bits. Set       */
    }                       /* bits_to_follow to zero.  */
}

/* ARITHMETIC DECODING ALGORITHM. */


/* CURRENT STATE OF THE DECODING. */

static code_value value;    /* Currently-seen code value                */
static code_value low, high;    /* Ends of current code region              */


/* START DECODING A STREAM OF SYMBOLS. */

start_decoding()
{   int i;
    value = 0;                  /* Input bits to fill the   */
    for (i = 1; i<=Code_value_bits; i++) {  /* code value.              */
        value = 2*value+input_bit();
    }
    low = 0;                    /* Full code range.         */
    high = Top_value;
}


/* DECODE THE NEXT SYMBOL. */

int decode_symbol(cum_freq)
    int cum_freq[];     /* Cumulative symbol frequencies            */
{   long range;         /* Size of current code region              */
    int cum;            /* Cumulative frequency calculated          */
    int symbol;         /* Symbol decoded                           */
    range = (long)(high-low)+1;
    cum =                   /* Find cum freq for value. */
      (((long)(value-low)+1)*cum_freq[0]-1)/range;
    for (symbol = 1; cum_freq[symbol]>cum; symbol++) ; /* Then find symbol. */
    high = low +                /* Narrow the code region   */
      (range*cum_freq[symbol-1])/cum_freq[0]-1; /* to that allotted to this */
    low = low +                 /* symbol.                  */
      (range*cum_freq[symbol])/cum_freq[0];
    for (;;) {                  /* Loop to get rid of bits. */
        if (high<Half) {
            /* nothing */           /* Expand low half.         */
        } 
        else if (low>=Half) {           /* Expand high half.        */
            value -= Half;
            low -= Half;            /* Subtract offset to top.  */
            high -= Half;
        }
        else if (low>=First_qtr         /* Expand middle half.      */
              && high<Third_qtr) {
            value -= First_qtr;
            low -= First_qtr;           /* Subtract offset to middle*/
            high -= First_qtr;
        }
        else break;             /* Otherwise exit loop.     */
        low = 2*low;
        high = 2*high+1;            /* Scale up code range.     */
        value = 2*value+input_bit();        /* Move in next input bit.  */
    }
    return symbol;
}

/* BIT INPUT ROUTINES. */

#include <stdio.h>

/* THE BIT BUFFER. */

static int buffer;      /* Bits waiting to be input                 */
static int bits_to_go;      /* Number of bits still in buffer           */
static int garbage_bits;    /* Number of bits past end-of-file          */


/* INITIALIZE BIT INPUT. */

start_inputing_bits()
{   bits_to_go = 0;             /* Buffer starts out with   */
    garbage_bits = 0;               /* no bits in it.           */
}


/* INPUT A BIT. */

int input_bit()
{   int t;
    if (bits_to_go==0) {            /* Read the next byte if no */
        buffer = getc(in);          /* bits are left in buffer. */
        if (buffer==EOF) {
            garbage_bits += 1;              /* Return arbitrary bits*/
            if (garbage_bits>Code_value_bits-2) {   /* after eof, but check */
                fprintf(stderr,"Bad input file\n"); /* for too many such.   */
                exit(-1);
            }
        }
        bits_to_go = 8;
    }
    t = buffer&1;               /* Return the next bit from */
    buffer >>= 1;               /* the bottom of the byte.  */
    bits_to_go -= 1;
    return t;
}
/* BIT OUTPUT ROUTINES. */

#include <stdio.h>


/* THE BIT BUFFER. */

static int buffer;      /* Bits buffered for output                 */
static int bits_to_go;      /* Number of bits free in buffer            */


/* INITIALIZE FOR BIT OUTPUT. */

start_outputing_bits()
{   buffer = 0;                 /* Buffer is empty to start */
    bits_to_go= 8;              /* with.                    */
}


/* OUTPUT A BIT. */

output_bit(bit)
    int bit;
{   buffer >>= 1; if (bit) buffer |= 0x80;  /* Put bit in top of buffer.*/
    bits_to_go -= 1;
    if (bits_to_go==0) {            /* Output buffer if it is   */
        putc(buffer,out);           /* now full.                */
        bits_to_go = 8;
    }
}


/* FLUSH OUT THE LAST BITS. */

done_outputing_bits()
{   putc(buffer>>bits_to_go,out);
}

/* MAIN PROGRAM FOR ENCODING. */

#include <stdio.h>

int main()
{
   in = fopen("software/benchmarks/data/input_very_very_small.asc", "r");
   out = fopen("software/benchmarks/data/_encadar.arc", "w+");

   start_model();               /* Set up other modules.    */
    start_outputing_bits();
    start_encoding();
    for (;;) {                  /* Loop through characters. */
        int ch; int symbol;
        ch = getc(in);          /* Read the next character. */
        if (ch==EOF) break;         /* Exit loop on end-of-file.*/
        symbol = char_to_index[ch];     /* Translate to an index.   */
        encode_symbol(symbol,cum_freq);     /* Encode that symbol.      */
        update_model(symbol);           /* Update the model.        */
    }
    encode_symbol(EOF_symbol,cum_freq);     /* Encode the EOF symbol.   */
    done_encoding();                /* Send the last few bits.  */
    done_outputing_bits();
    exit(0);
    return 0;
}
