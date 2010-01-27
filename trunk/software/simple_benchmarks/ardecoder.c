#include <stdlib.h>
#include <stdio.h>

FILE * in;
FILE * out;

/* INTERFACE TO THE MODEL. */

/* (C) Copyright 1987, Radford Neal. 

   Permission is granted to use this program for research purposes. Please
   note that this code was written to provide performance figures. I am
   well aware that it is not documented to the standards of production
   code. You're on your own. */



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

/* (C) Copyright 1987, Radford Neal. 

   Permission is granted to use this program for research purposes. Please
   note that this code was written to provide performance figures. I am
   well aware that it is not documented to the standards of production
   code. You're on your own. */


/* SIZE OF ARITHMETIC CODE VALUES. */

#define Code_value_bits 16      /* Number of bits in a code value   */
typedef long code_value;        /* Type of an arithmetic code value */

#define Top_value (((long)1<<Code_value_bits)-1)      /* Largest code value */


/* HALF AND QUARTER POINTS IN THE CODE VALUE RANGE. */

#define First_qtr (Top_value/4+1)   /* Point after first quarter        */
#define Half      (2*First_qtr)     /* Point after first half           */
#define Third_qtr (3*First_qtr)     /* Point after third quarter        */
/* ARITHMETIC ENCODING ALGORITHM. */

/* (C) Copyright 1987, Radford Neal. 

   Permission is granted to use this program for research purposes. Please
   note that this code was written to provide performance figures. I am
   well aware that it is not documented to the standards of production
   code. You're on your own. */


#include <stdio.h>


/* THE BIT BUFFER. */

static int S_buffer;        /* Bits buffered for output                 */
static int S_bits_to_go;    /* Number of bits free in buffer            */


/* INITIALIZE FOR BIT OUTPUT. */

start_outputing_bits()
{   S_buffer = 0;               /* Buffer is empty to start */
    S_bits_to_go= 8;                /* with.                    */
}


/* OUTPUT A BIT. */

#define output_bit(bit) do \
{   buffer >>= 1; if (bit) buffer |= 0x80; \
    bits_to_go -= 1; \
    if (bits_to_go==0) { \
        putc(buffer,out); \
        bits_to_go = 8; \
    } \
} while (0)


/* FLUSH OUT THE LAST BITS. */

done_outputing_bits()
{   putc(S_buffer>>S_bits_to_go,out);
}


/* OUTPUT BITS PLUS FOLLOWING OPPOSITE BITS. */

#define bit_plus_follow(bit) do \
{   output_bit(bit); \
    while (bits_to_follow>0) { \
        output_bit(!bit); \
        bits_to_follow -= 1; \
    } \
} while(0)


/* CURRENT STATE OF THE ENCODING. */

static code_value S_low, S_high;/* Ends of the current code region          */
static long S_bits_to_follow;   /* Number of opposite bits to output after  */
                /* the next bit.                            */


/* START ENCODING A STREAM OF SYMBOLS. */

start_encoding()
{   S_low = 0;                  /* Full code range.         */
    S_high = Top_value;
    S_bits_to_follow = 0;           /* No bits to follow next.  */
}


/* ENCODE A SYMBOL. */

encode_symbol(symbol,P_cum_freq)
    int symbol;         /* Symbol to encode                         */
    int P_cum_freq[];       /* Cumulative symbol frequencies            */
{   register code_value low, high;
    {   register long range;
        register int *cum_freq;
        cum_freq = P_cum_freq;
        range = (long)(S_high-S_low)+1;
        high = S_low +              /* Narrow the code region   */
          (range*cum_freq[symbol-1])/cum_freq[0]-1;/* to that allotted to   */
        low = S_low +               /* this symbol.             */
        (range*cum_freq[symbol])/cum_freq[0];
    }
    {   register int H;
        register long bits_to_follow;
        register int buffer, bits_to_go;
        H = Half;
        bits_to_follow = S_bits_to_follow;
        buffer = S_buffer; bits_to_go = S_bits_to_go;
        for (;;) {              /* Loop to output bits.     */
            if (high<H) {
                bit_plus_follow(0);     /* Output 0 if in low half. */
            } 
            else if (low>=H) {          /* Output 1 if in high half.*/
                bit_plus_follow(1);
                low -= H;
                high -= H;          /* Subtract offset to top.  */
            }
            else if (low>=First_qtr     /* Output an opposite bit   */
                  && high<Third_qtr) {      /* later if in middle half. */
                bits_to_follow += 1;
                low -= First_qtr;       /* Subtract offset to middle*/
                high -= First_qtr;
            }
            else break;             /* Otherwise exit loop.     */
            low += low;
            high += high; high += 1;        /* Scale up code range.     */
        }
        S_bits_to_follow = bits_to_follow;
        S_buffer = buffer; S_bits_to_go = bits_to_go;
    }
    S_low = low; S_high = high; 
}


/* FINISH ENCODING THE STREAM. */

done_encoding()
{   register code_value low, high;
    register long bits_to_follow;
    register int buffer, bits_to_go;
    low = S_low; high = S_high; 
    bits_to_follow = S_bits_to_follow;
    buffer = S_buffer; bits_to_go = S_bits_to_go;
    bits_to_follow += 1;            /* Output two bits that     */
    if (S_low<First_qtr) bit_plus_follow(0);    /* select the quarter that  */
    else bit_plus_follow(1);            /* the current code range   */
    S_low = low; S_high = high;         /* contains.                */
    S_bits_to_follow = bits_to_follow;
    S_buffer = buffer; S_bits_to_go = bits_to_go;
}
/* ARITHMETIC DECODING ALGORITHM. */

/* (C) Copyright 1987, Radford Neal. 

   Permission is granted to use this program for research purposes. Please
   note that this code was written to provide performance figures. I am
   well aware that it is not documented to the standards of production
   code. You're on your own. */


#include <stdio.h>


/* THE BIT BUFFER. */

static int S_buffer;        /* Bits waiting to be input                 */
static int S_bits_to_go;    /* Number of bits still in buffer           */


/* INITIALIZE BIT INPUT. */

start_inputing_bits()
{   S_bits_to_go = 0;               /* Buffer starts out with   */
}                       /* no bits in it.           */


/* INPUT A BIT. */

#define add_input_bit(v) do \
{   bits_to_go -= 1; \
    if (bits_to_go<0) { \
        buffer = getc(in); \
        bits_to_go = 7; \
    } \
    v += buffer&1; \
    buffer >>= 1; \
} while (0)


/* CURRENT STATE OF THE DECODING. */

static code_value S_value;  /* Currently-seen code value                */
static code_value S_low, S_high;/* Ends of current code region              */


/* START DECODING A STREAM OF SYMBOLS. */

start_decoding()
{   register int i;
    register int buffer, bits_to_go;
    buffer = S_buffer; bits_to_go = S_bits_to_go;
    S_value = 0;                /* Input bits to fill the   */
    for (i = 1; i<=Code_value_bits; i++) {  /* code value.              */
        S_value += S_value; add_input_bit(S_value);
    }
    S_low = 0;                  /* Full code range.         */
    S_high = Top_value;
    S_buffer = buffer; S_bits_to_go = bits_to_go;
}


/* DECODE THE NEXT SYMBOL. */

int decode_symbol(P_cum_freq)
    int P_cum_freq[];       /* Cumulative symbol frequencies            */
{   int symbol; 
    register code_value low, high, value;
    low = S_low; high = S_high; value = S_value;
    {   register int *cum_freq;
        register int cum;
        register int *p;            
        long range;
        cum_freq = P_cum_freq;
        range = (long)(high-low)+1;
        cum =                   /* Find cum freq for value. */
          (((long)(value-low)+1)*cum_freq[0]-1)/range;
        p = &cum_freq[1];           /* Then find symbol.        */
        while (*p++>cum) ;
        symbol = p-&cum_freq[1];
        high = low +                /* Narrow the code region   */
          (range*cum_freq[symbol-1])/cum_freq[0]-1;/* to that allotted to   */
        low = low +                 /* tis symbol.              */
          (range*cum_freq[symbol])/cum_freq[0];
    }
    {   register int H;
        register int buffer, bits_to_go;
        buffer = S_buffer; bits_to_go = S_bits_to_go;
        H = Half;
        for (;;) {              /* Loop to get rid of bits. */
            if (high<H) {
                /* nothing */           /* Expand low half.         */
            } 
            else if (low>=H) {          /* Expand high half.        */
                value -= H;
                low -= H;           /* Subtract offset to top.  */
                high -= H;
            }
            else if (low>=First_qtr     /* Expand middle half.      */
                  && high<Third_qtr) {
                value -= First_qtr;
                low -= First_qtr;       /* Subtract offset to middle*/
                high -= First_qtr;
            }
            else break;             /* Otherwise exit loop.     */
            low += low;
            high += high; high += 1;        /* Scale up code range.     */
            value += value;         /* Move in next input bit.  */
            add_input_bit(value);
        }
        S_buffer = buffer; S_bits_to_go = bits_to_go;
    }
    S_low = low; S_high = high; S_value = value;
    return symbol;
}

/* THE ADAPTIVE SOURCE MODEL */

/* (C) Copyright 1987, Radford Neal. 

   Permission is granted to use this program for research purposes. Please
   note that this code was written to provide performance figures. I am
   well aware that it is not documented to the standards of production
   code. You're on your own. */


int freq[No_of_symbols+1];  /* Symbol frequencies                     */


/* INITIALIZE THE MODEL. */

start_model()
{   register int i;
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
    register int symbol;    /* Index of new symbol                      */
{   register int i;     /* New index for symbol                     */
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
    { register int *p;              /* count for the symbol and */
      p = &cum_freq[i];             /* update the cumulative    */
      do { *--p += 1; } while (p>&cum_freq[0]); /* frequencies.             */
    }
}

/* MAIN PROGRAM FOR ENCODING. */

/* (C) Copyright 1987, Radford Neal. 

   Permission is granted to use this program for research purposes. Please
   note that this code was written to provide performance figures. I am
   well aware that it is not documented to the standards of production
   code. You're on your own. */


#include <stdio.h>

main()
{
   in = fopen("software/benchmarks/data/arcomp.arc", "r");
   out = fopen("software/benchmarks/data/ardecoded.out", "w+");

   start_model();               /* Set up other modules.    */
    start_inputing_bits();
    start_decoding();
    for (;;) {                  /* Loop through characters. */
        register int ch; register int symbol;
        symbol = decode_symbol(cum_freq);   /* Decode next symbol.      */
        if (symbol==EOF_symbol) break;      /* Exit loop if EOF symbol. */
        ch = index_to_char[symbol];     /* Translate to a character.*/
        putc(ch,out);           /* Write that character.    */
        update_model(symbol);           /* Update the model.        */
    }
    exit(0);
}
