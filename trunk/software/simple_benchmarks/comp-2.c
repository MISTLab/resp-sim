/*
 * Listing 10 -- comp-2.c
 *
 * This module is the driver program for a variable order
 * finite context compression program.  The maximum order is
 * determined by command line option.  This particular version
 * also monitors compression ratios, and flushes the model whenever
 * the local (last 256 symbols) compression ratio hits 90% or higher.
 *
 * To build this program:
 *
 * Turbo C:     tcc -w -mc comp-2.c model-2.c bitio.c coder.c
 * QuickC:      qcl /AC /W3 comp-2.c model-2.c bitio.c coder.c
 * Zortech:     ztc -mc comp-2.c model-2.c bitio.c coder.c
 * *NIX:        cc -o comp-2 comp-2.c model-2.c bitio.c coder.c
 *
 * Command line options:
 *
 *  -f text_file_name  [defaults to test.inp]
 *  -c compressed_file_name [defaults to test.cmp]
 *  -o order [defaults to 3 for model-2]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * Listing 8 -- model.h
 *
 * This file contains all of the function prototypes and
 * external variable declarations needed to interface with
 * the modeling code found in model-1.c or model-2.c.
 */

/*
 * Eternal variable declarations.
 */
extern int max_order;
extern int flushing_enabled;
/*
 * Prototypes for routines that can be called from MODEL-X.C
 */
void initialize_model( void );
void update_model( int symbol );
int convert_int_to_symbol( int symbol, SYMBOL *s );
void get_symbol_scale( SYMBOL *s );
int convert_symbol_to_int( int count, SYMBOL *s );
void add_character_to_model( int c );
void flush_model( void );

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
 * Listing 12 -- model-2.c
 *
 * This module contains all of the modeling functions used with
 * comp-2.c and expand-2.c.  This modeling unit keeps track of
 * all contexts from 0 up to max_order, which defaults to 3.
 * In addition, there is a special context -1 which is a fixed model
 * used to encode previously unseen characters, and a context -2
 * which is used to encode EOF and FLUSH messages.
 *
 * Each context is stored in a special CONTEXT structure, which is
 * documented below.  Context tables are not created until the
 * context is seen, and they are never destroyed.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * This program consumes massive amounts of memory.  One way to
 * handle large amounts of memory is to use Zortech's __handle
 * pointer type.  So that my code will run with other compilers
 * as well, the handle stuff gets redefined when using other
 * compilers.
 */
#ifdef __ZTC__
#include <handle.h>
#else
#define __handle
#define handle_calloc( a )        calloc( (a), 1 )
#define handle_realloc( a, b )    realloc( (a), (b) )
#define handle_free( a )          free( (a) )
#endif

/* A context table contains a list of the counts for all symbols
 * that have been seen in the defined context.  For example, a
 * context of "Zor" might have only had 2 different characters
 * appear.  't' might have appeared 10 times, and 'l' might have
 * appeared once.  These two counts are stored in the context
 * table.  The counts are stored in the STATS structure.  All of
 * the counts for a given context are stored in and array of STATS.
 * As new characters are added to a particular contexts, the STATS
 * array will grow.  Sometimes, the STATS array will shrink
 * after flushing the model.
 */
typedef struct {
                unsigned char symbol;
                unsigned char counts;
               } STATS;

/*
 * Each context has to have links to higher order contexts.  These
 * links are used to navigate through the context tables.  For example,
 * to find the context table for "ABC", I start at the order 0 table,
 * then find the pointer to the "A" context table by looking through
 * then LINKS array.  At that table, we find the "B" link and go to
 * that table.  The process continues until the destination table is
 * found.  The table pointed to by the LINKS array corresponds to the
 * symbol found at the same offset in the STATS table.  The reason that
 * LINKS is in a separate structure instead of being combined with
 * STATS is to save space.  All of the leaf context nodes don't need
 * next pointers, since they are in the highest order context.  In the
 * leaf nodes, the LINKS array is a NULL pointers.
 */
typedef struct {
                 struct context *next;
               } LINKS;

/*
 * The CONTEXT structure holds all of the know information about
 * a particular context.  The links and stats pointers are discussed
 * immediately above here.  The max_index element gives the maximum
 * index that can be applied to the stats or link array.  When the
 * table is first created, and stats is set to NULL, max_index is set
 * to -1.  As soon as single element is added to stats, max_index is
 * incremented to 0.
 *
 * The lesser context pointer is a navigational aid.  It points to
 * the context that is one less than the current order.  For example,
 * if the current context is "ABC", the lesser_context pointer will
 * point to "BC".  The reason for maintaining this pointer is that
 * this particular bit of table searching is done frequently, but
 * the pointer only needs to be built once, when the context is
 * created.
 */
typedef struct context {
                         int max_index;
                         LINKS __handle *links;
                         STATS __handle *stats;
                         struct context *lesser_context;
                       } CONTEXT;
/*
 * max_order is the maximum order that will be maintained by this
 * program.  EXPAND-2 and COMP-2 both will modify this int based
 * on command line parameters.
 */
int max_order=3;
/*
 * *contexts[] is an array of current contexts.  If I want to find
 * the order 0 context for the current state of the model, I just
 * look at contexts[0].  This array of context pointers is set up
 * every time the model is updated.
 */
CONTEXT **contexts;
/*
 * current_order contains the current order of the model.  It starts
 * at max_order, and is decremented every time an ESCAPE is sent.  It
 * will only go down to -1 for normal symbols, but can go to -2 for
 * EOF and FLUSH.
 */
int current_order;
/*
 * This variable tells COMP-2.C that the FLUSH symbol can be
 * sent using this model.
 */
int flushing_enabled=1;
/*
 * This table contains the cumulative totals for the current context.
 * Because this program is using exclusion, totals has to be calculated
 * every time a context is used.  The scoreboard array keeps track of
 * symbols that have appeared in higher order models, so that they
 * can be excluded from lower order context total calculations.
 */
short int totals[ 258 ];
char scoreboard[ 256 ];

/*
 * Local procedure declarations.
 */
void error_exit( char *message );
void update_table( CONTEXT *table, unsigned char symbol );
void rescale_table( CONTEXT *table );
void totalize_table( CONTEXT *table );
CONTEXT *shift_to_next_context( CONTEXT *table, unsigned char c, int order);
CONTEXT *allocate_next_order_table( CONTEXT *table,
                                    unsigned char symbol,
                                    CONTEXT *lesser_context );

/*
 * This routine has to get everything set up properly so that
 * the model can be maintained properly.  The first step is to create
 * the *contexts[] array used later to find current context tables.
 * The *contexts[] array indices go from -2 up to max_order, so
 * the table needs to be fiddled with a little.  This routine then
 * has to create the special order -2 and order -1 tables by hand,
 * since they aren't quite like other tables.  Then the current
 * context is set to \0, \0, \0, ... and the appropriate tables
 * are built to support that context.  The current order is set
 * to max_order, the scoreboard is cleared, and the system is
 * ready to go.
 */

void initialize_model()
{
    int i;
    CONTEXT *null_table;
    CONTEXT *control_table;

    current_order = max_order;
    contexts = (CONTEXT **) calloc( sizeof( CONTEXT * ), 10 );
    if ( contexts == NULL )
        error_exit( "Failure #1: allocating context table!" );
    contexts += 2;
    null_table = (CONTEXT *) calloc( sizeof( CONTEXT ), 1 );
    if ( null_table == NULL )
        error_exit( "Failure #2: allocating null table!" );
    null_table->max_index = -1;
    contexts[ -1 ] = null_table;
    for ( i = 0 ; i <= max_order ; i++ )
        contexts[ i ] = allocate_next_order_table( contexts[ i-1 ],
                                               0,
                                               contexts[ i-1 ] );
    handle_free( (char __handle *) null_table->stats );
    null_table->stats =
         (STATS __handle *) handle_calloc( sizeof( STATS ) * 256 );
    if ( null_table->stats == NULL )
        error_exit( "Failure #3: allocating null table!" );
    null_table->max_index = 255;
    for ( i=0 ; i < 256 ; i++ )
    {
        null_table->stats[ i ].symbol = (unsigned char) i;
        null_table->stats[ i ].counts = 1;
    }

    control_table = (CONTEXT *) calloc( sizeof(CONTEXT), 1 );
    if ( control_table == NULL )
        error_exit( "Failure #4: allocating null table!" );
    control_table->stats =
         (STATS __handle *) handle_calloc( sizeof( STATS ) * 2 );
    if ( control_table->stats == NULL )
        error_exit( "Failure #5: allocating null table!" );
    contexts[ -2 ] = control_table;
    control_table->max_index = 1;
    control_table->stats[ 0 ].symbol = -FLUSH;
    control_table->stats[ 0 ].counts = 1;
    control_table->stats[ 1 ].symbol =- DONE;
    control_table->stats[ 1 ].counts = 1;

    for ( i = 0 ; i < 256 ; i++ )
        scoreboard[ i ] = 0;
}
/*
 * This is a utility routine used to create new tables when a new
 * context is created.  It gets a pointer to the current context,
 * and gets the symbol that needs to be added to it.  It also needs
 * a pointer to the lesser context for the table that is to be
 * created.  For example, if the current context was "ABC", and the
 * symbol 'D' was read in, add_character_to_model would need to
 * create the new context "BCD".  This routine would get called
 * with a pointer to "BC", the symbol 'D', and a pointer to context
 * "CD".  This routine then creates a new table for "BCD", adds it
 * to the link table for "BC", and gives "BCD" a back pointer to
 * "CD".  Note that finding the lesser context is a difficult
 * task, and isn't done here.  This routine mainly worries about
 * modifying the stats and links fields in the current context.
 */

CONTEXT *allocate_next_order_table( CONTEXT *table,
                                    unsigned char symbol,
                                    CONTEXT *lesser_context )
{
    CONTEXT *new_table;
    int i;
    unsigned int new_size;

    for ( i = 0 ; i <= table->max_index ; i++ )
        if ( table->stats[ i ].symbol == symbol )
            break;
    if ( i > table->max_index )
    {
        table->max_index++;
        new_size = sizeof( LINKS );
        new_size *= table->max_index + 1;
        if ( table->links == NULL )
            table->links = (LINKS __handle *) handle_calloc( new_size );
        else
            table->links = (LINKS __handle *)
                 handle_realloc( (char __handle *) table->links, new_size );
        new_size = sizeof( STATS );
        new_size *= table->max_index + 1;
        if ( table->stats == NULL )
            table->stats = (STATS __handle *) handle_calloc( new_size );
        else
            table->stats = (STATS __handle *)
                handle_realloc( (char __handle *) table->stats, new_size );
        if ( table->links == NULL )
            error_exit( "Failure #6: allocating new table" );
        if ( table->stats == NULL )
            error_exit( "Failure #7: allocating new table" );
        table->stats[ i ].symbol = symbol;
        table->stats[ i ].counts = 0;
    }
    new_table = (CONTEXT *) calloc( sizeof( CONTEXT ), 1 );
    if ( new_table == NULL )
        error_exit( "Failure #8: allocating new table" );
    new_table->max_index = -1;
    table->links[ i ].next = new_table;
    new_table->lesser_context = lesser_context;
    return( new_table );
}

/*
 * This routine is called to increment the counts for the current
 * contexts.  It is called after a character has been encoded or
 * decoded.  All it does is call update_table for each of the
 * current contexts, which does the work of incrementing the count.
 * This particular version of update_model() practices update exclusion,
 * which means that if lower order models weren't used to encode
 * or decode the character, they don't get their counts updated.
 * This seems to improve compression performance quite a bit.
 * To disable update exclusion, the loop would be changed to run
 * from 0 to max_order, instead of current_order to max_order.
 */
void update_model( int symbol )
{
    int i;
    int local_order;

    if ( current_order < 0 )
        local_order = 0;
    else
        local_order = current_order;
    if ( symbol >= 0 )
    {
        while ( local_order <= max_order )
        {
            if ( symbol >= 0 )
                update_table( contexts[ local_order ], (unsigned char) symbol );
            local_order++;
        }
    }
    current_order = max_order;
    for ( i = 0 ; i < 256 ; i++ )
        scoreboard[ i ] = 0;
}
/*
 * This routine is called to update the count for a particular symbol
 * in a particular table.  The table is one of the current contexts,
 * and the symbol is the last symbol encoded or decoded.  In principle
 * this is a fairly simple routine, but a couple of complications make
 * things a little messier.  First of all, the given table may not
 * already have the symbol defined in its statistics table.  If it
 * doesn't, the stats table has to grow and have the new guy added
 * to it.  Secondly, the symbols are kept in sorted order by count
 * in the table so as that the table can be trimmed during the flush
 * operation.  When this symbol is incremented, it might have to be moved
 * up to reflect its new rank.  Finally, since the counters are only
 * bytes, if the count reaches 255, the table absolutely must be rescaled
 * to get the counts back down to a reasonable level.
 */
void update_table( CONTEXT *table, unsigned char symbol )
{
    int i;
    int index;
    unsigned char temp;
    CONTEXT *temp_ptr;
    unsigned int new_size;
/*
 * First, find the symbol in the appropriate context table.  The first
 * symbol in the table is the most active, so start there.
 */
    index = 0;
    while ( index <= table->max_index &&
            table->stats[index].symbol != symbol )
        index++;
    if ( index > table->max_index )
    {
        table->max_index++;
        new_size = sizeof( LINKS );
        new_size *= table->max_index + 1;
        if ( current_order < max_order )
        {
            if ( table->max_index == 0 )
                table->links = (LINKS __handle *) handle_calloc( new_size );
            else
                table->links = (LINKS __handle *)
                   handle_realloc( (char __handle *) table->links, new_size );
            if ( table->links == NULL )
                error_exit( "Error #9: reallocating table space!" );
            table->links[ index ].next = NULL;
        }
        new_size = sizeof( STATS );
        new_size *= table->max_index + 1;
        if (table->max_index==0)
            table->stats = (STATS __handle *) handle_calloc( new_size );
        else
            table->stats = (STATS __handle *)
                handle_realloc( (char __handle *) table->stats, new_size );
        if ( table->stats == NULL )
            error_exit( "Error #10: reallocating table space!" );
        table->stats[ index ].symbol = symbol;
        table->stats[ index ].counts = 0;
    }
/*
 * Now I move the symbol to the front of its list.
 */
    i = index;
    while ( i > 0 &&
            table->stats[ index ].counts == table->stats[ i-1 ].counts )
        i--;
    if ( i != index )
    {
        temp = table->stats[ index ].symbol;
        table->stats[ index ].symbol = table->stats[ i ].symbol;
        table->stats[ i ].symbol = temp;
        if ( table->links != NULL )
        {
            temp_ptr = table->links[ index ].next;
            table->links[ index ].next = table->links[ i ].next;
            table->links[ i ].next = temp_ptr;
        }
        index = i;
    }
/*
 * The switch has been performed, now I can update the counts
 */
    table->stats[ index ].counts++;
    if ( table->stats[ index ].counts == 255 )
        rescale_table( table );
}

/*
 * This routine is called when a given symbol needs to be encoded.
 * It is the job of this routine to find the symbol in the context
 * table associated with the current table, and return the low and
 * high counts associated with that symbol, as well as the scale.
 * Finding the table is simple.  Unfortunately, once I find the table,
 * I have to build the table of cumulative counts, which is
 * expensive, and is done elsewhere.  If the symbol is found in the
 * table, the appropriate counts are returned.  If the symbols is
 * not found, the ESCAPE symbol probabilities are returned, and
 * the current order is reduced.  Note also the kludge to support
 * the order -2 character set, which consists of negative numbers
 * instead of unsigned chars.  This insures that no match will every
 * be found for the EOF or FLUSH symbols in any "normal" table.
 */
int convert_int_to_symbol( int c, SYMBOL *s )
{
    int i;
    CONTEXT *table;

    table = contexts[ current_order ];
    totalize_table( table );
    s->scale = totals[ 0 ];
    if ( current_order == -2 )
        c = -c;
    for ( i = 0 ; i <= table->max_index ; i++ )
    {
        if ( c == (int) table->stats[ i ].symbol )
        {
            if ( table->stats[ i ].counts == 0 )
                break;
            s->low_count = totals[ i+2 ];
            s->high_count = totals[ i+1 ];
            return( 0 );
        }
    }
    s->low_count = totals[ 1 ];
    s->high_count = totals[ 0 ];
    current_order--;
    return( 1 );
}
/*
 * This routine is called when decoding an arithmetic number.  In
 * order to decode the present symbol, the current scale in the
 * model must be determined.  This requires looking up the current
 * table, then building the totals table.  Once that is done, the
 * cumulative total table has the symbol scale at element 0.
 */
void get_symbol_scale( SYMBOL *s )
{
    CONTEXT *table;

    table = contexts[ current_order ];
    totalize_table( table );
    s->scale = totals[ 0 ];
}

/*
 * This routine is called during decoding.  It is given a count that
 * came out of the arithmetic decoder, and has to find the symbol that
 * matches the count.  The cumulative totals are already stored in the
 * totals[] table, form the call to get_symbol_scale, so this routine
 * just has to look through that table.  Once the match is found,
 * the appropriate character is returned to the caller.  Two possible
 * complications.  First, the character might be the ESCAPE character,
 * in which case the current_order has to be decremented.  The other
 * complication is that the order might be -2, in which case we return
 * the negative of the symbol so it isn't confused with a normal
 * symbol.
 */
int convert_symbol_to_int( int count, SYMBOL *s)
{
    int c;
    CONTEXT *table;

    table = contexts[ current_order ];
    for ( c = 0; count < totals[ c ] ; c++ )
        ;
    s->high_count = totals[ c-1 ];
    s->low_count = totals[ c ];
    if ( c == 1 )
    {
        current_order--;
        return( ESCAPE );
    }
    if ( current_order < -1 )
        return( (int) -table->stats[ c-2 ].symbol );
    else
        return( table->stats[ c-2 ].symbol );
}


/*
 * After the model has been updated for a new character, this routine
 * is called to "shift" into the new context.  For example, if the
 * last context was "ABC", and the symbol 'D' had just been processed,
 * this routine would want to update the context pointers to that
 * contexts[1]=="D", contexts[2]=="CD" and contexts[3]=="BCD".  The
 * potential problem is that some of these tables may not exist.
 * The way this is handled is by the shift_to_next_context routine.
 * It is passed a pointer to the "ABC" context, along with the symbol
 * 'D', and its job is to return a pointer to "BCD".  Once we have
 * "BCD", we can follow the lesser context pointers in order to get
 * the pointers to "CD" and "C".  The hard work was done in
 * shift_to_context().
 */
void add_character_to_model( int c )
{
    int i;
    if ( max_order < 0 || c < 0 )
       return;
    contexts[ max_order ] =
       shift_to_next_context( contexts[ max_order ],
                              (unsigned char) c, max_order );
    for ( i = max_order-1 ; i > 0 ; i-- )
        contexts[ i ] = contexts[ i+1 ]->lesser_context;
}

/*
 * This routine is called when adding a new character to the model. From
 * the previous example, if the current context was "ABC", and the new
 * symbol was 'D', this routine would get called with a pointer to
 * context table "ABC", and symbol 'D', with order max_order.  What this
 * routine needs to do then is to find the context table "BCD".  This
 * should be an easy job, and it is if the table already exists.  All
 * we have to in that case is follow the back pointer from "ABC" to "BC".
 * We then search the link table of "BC" until we find the linke to "D".
 * That link points to "BCD", and that value is then returned to the
 * caller.  The problem crops up when "BC" doesn't have a pointer to
 * "BCD".  This generally means that the "BCD" context has not appeared
 * yet.  When this happens, it means a new table has to be created and
 * added to the "BC" table.  That can be done with a single call to
 * the allocate_new_table routine.  The only problem is that the
 * allocate_new_table routine wants to know what the lesser context for
 * the new table is going to be.  In other words, when I create "BCD",
 * I need to know where "CD" is located.  In order to find "CD", I
 * have to recursively call shift_to_next_context, passing it a pointer
 * to context "C" and they symbol 'D'.  It then returns a pointer to
 * "CD", which I use to create the "BCD" table.  The recursion is guaranteed
 * to end if it ever gets to order -1, because the null table is
 * guaranteed to have a for every symbol to the order 0 table.  This is
 * the most complicated part of the modeling program, but it is
 * necessary for performance reasons.
 */
CONTEXT *shift_to_next_context( CONTEXT *table, unsigned char c, int order)
{
    int i;
    CONTEXT *new_lesser;
/*
 * First, try to find the new context by backing up to the lesser
 * context and searching its link table.  If I find the link, we take
 * a quick and easy exit, returning the link.  Note that their is a
 * special Kludge for context order 0.  We know for a fact that
 * the lesser context pointer at order 0 points to the null table,
 * order -1, and we know that the -1 table only has a single link
 * pointer, which points back to the order 0 table.
 */
    table = table->lesser_context;
    if ( order == 0 )
        return( table->links[ 0 ].next );
    for ( i = 0 ; i <= table->max_index ; i++ )
        if ( table->stats[ i ].symbol == c )
            if ( table->links[ i ].next != NULL )
                return( table->links[ i ].next );
            else
                break;
/*
 * If I get here, it means the new context did not exist.  I have to
 * create the new context, add a link to it here, and add the backwards
 * link to *his* previous context.  Creating the table and adding it to
 * this table is pretty easy, but adding the back pointer isn't.  Since
 * creating the new back pointer isn't easy, I duck my responsibility
 * and recurse to myself in order to pick it up.
 */
    new_lesser = shift_to_next_context( table, c, order-1 );
/*
 * Now that I have the back pointer for this table, I can make a call
 * to a utility to allocate the new table.
 */
    table = allocate_next_order_table( table, c, new_lesser );
    return( table );
}

/*
 * Rescaling the table needs to be done for one of three reasons.
 * First, if the maximum count for the table has exceeded 16383, it
 * means that arithmetic coding using 16 and 32 bit registers might
 * no longer work.  Secondly, if an individual symbol count has
 * reached 255, it will no longer fit in a byte.  Third, if the
 * current model isn't compressing well, the compressor program may
 * want to rescale all tables in order to give more weight to newer
 * statistics.  All this routine does is divide each count by 2.
 * If any counts drop to 0, the counters can be removed from the
 * stats table, but only if this is a leaf context.  Otherwise, we
 * might cut a link to a higher order table.
 */
void rescale_table( CONTEXT *table )
{
    int i;

    if ( table->max_index == -1 )
        return;
    for ( i = 0 ; i <= table->max_index ; i++ )
        table->stats[ i ].counts /= 2;
    if ( table->stats[ table->max_index ].counts == 0 &&
         table->links == NULL )
    {
        while ( table->stats[ table->max_index ].counts == 0 &&
                table->max_index >= 0 )
            table->max_index--;
        if ( table->max_index == -1 )
        {
            handle_free( (char __handle *) table->stats );
            table->stats = NULL;
        }
        else
        {
            table->stats = (STATS __handle *)
                handle_realloc( (char __handle *) table->stats,
                                 sizeof( STATS ) * ( table->max_index + 1 ) );
            if ( table->stats == NULL )
                error_exit( "Error #11: reallocating stats space!" );
        }
    }
}

/*
 * This routine has the job of creating a cumulative totals table for
 * a given context.  The cumulative low and high for symbol c are going to
 * be stored in totals[c+2] and totals[c+1].  Locations 0 and 1 are
 * reserved for the special ESCAPE symbol.  The ESCAPE symbol
 * count is calculated dynamically, and changes based on what the
 * current context looks like.  Note also that this routine ignores
 * any counts for symbols that have already showed up in the scoreboard,
 * and it adds all new symbols found here to the scoreboard.  This
 * allows us to exclude counts of symbols that have already appeared in
 * higher order contexts, improving compression quite a bit.
 */
void totalize_table( CONTEXT *table )
{
    int i;
    unsigned char max;

    for ( ; ; )
    {
        max = 0;
        i = table->max_index + 2;
        totals[ i ] = 0;
        for ( ; i > 1 ; i-- )
        {
            totals[ i-1 ] = totals[ i ];
            if ( table->stats[ i-2 ].counts )
                if ( ( current_order == -2 ) ||
                     scoreboard[ table->stats[ i-2 ].symbol ] == 0 )
                     totals[ i-1 ] += table->stats[ i-2 ].counts;
            if ( table->stats[ i-2 ].counts > max )
                max = table->stats[ i-2 ].counts;
        }
/*
 * Here is where the escape calculation needs to take place.
 */
        if ( max == 0 )
            totals[ 0 ] = 1;
        else
        {
            totals[ 0 ] = (short int) ( 256 - table->max_index );
            totals[ 0 ] *= table->max_index;
            totals[ 0 ] /= 256;
            totals[ 0 ] /= max;
            totals[ 0 ]++;
            totals[ 0 ] += totals[ 1 ];
        }
        if ( totals[ 0 ] < MAXIMUM_SCALE )
            break;
        rescale_table( table );
    }
    for ( i = 0 ; i < table->max_index ; i++ )
    if (table->stats[i].counts != 0)
            scoreboard[ table->stats[ i ].symbol ] = 1;
}

/*
 * This routine is called when the entire model is to be flushed.
 * This is done in an attempt to improve the compression ratio by
 * giving greater weight to upcoming statistics.  This routine
 * starts at the given table, and recursively calls itself to
 * rescale every table in its list of links.  The table itself
 * is then rescaled.
 */
void recursive_flush( CONTEXT *table )
{
    int i;

    if ( table->links != NULL )
        for ( i = 0 ; i <= table->max_index ; i++ )
            if ( table->links[ i ].next != NULL )
                recursive_flush( table->links[ i ].next );
    rescale_table( table );
}

/*
 * This routine is called to flush the whole table, which it does
 * by calling the recursive flush routine starting at the order 0
 * table.
 */
void flush_model()
{
    recursive_flush( contexts[ 0 ] );
}

void error_exit( char *message)
{
    putc( '\n', stdout );
    puts( message );
    exit( -1 );
}


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
 * The file pointers are used throughout this module.
 */
FILE *text_file;
FILE *compressed_file;

/*
 * Declarations for local procedures.
 */
void initialize_options( int argc, char **argv );
int check_compression( void );
void print_compression( void );

/*
 * The main procedure is similar to the main found in COMP-1.C.
 * It has to initialize the coder, the bit oriented I/O, the
 * standard I/O, and the model.  It then sits in a loop reading
 * input symbols and encoding them.  One difference is that every
 * 256 symbols a compression check is performed.  If the compression
 * ratio exceeds 90%, a flush character is encoded.  This flushes
 * the encoding model, and will cause the decoder to flush its model
 * when the file is being expanded.  The second difference is that
 * each symbol is repeatedly encoded until a succesfull encoding
 * occurs.  When trying to encode a character in a particular order,
 * the model may have to transmit an ESCAPE character.  If this
 * is the case, the character has to be retransmitted using a lower
 * order.  This process repeats until a succesful match is found of
 * the symbol in a particular context.  Usually this means going down
 * no further than the order -1 model.  However, the FLUSH and DONE
 * symbols do drop back to the order -2 model.  Note also that by
 * all rights, add_character_to_model() and update_model() logically
 * should be combined into a single routine.
 */
int main( int argc, char **argv )
{
    SYMBOL s;
    int c;
    int escaped;
    int flush = 0;
    long int text_count = 0;

/*    text_file = fopen("../software/benchmarks/data/input_very_very_small.asc", "rb" );
    compressed_file = fopen( "../software/benchmarks/data/comp2.cmp", "wb" );*/
    text_file = fopen("../software/benchmarks/data/input_very_very_small.asc", "rb" );
    compressed_file = fopen( "../software/benchmarks/data/comp2.cmp", "wb" );
    max_order = 3;
    if ( text_file == NULL || compressed_file == NULL )
    {
        printf( "Had trouble opening one of the files!\n" );
        exit( -1 );
    }
    setvbuf( text_file, NULL, _IOFBF, 4096 );
    setbuf( stdout, NULL );

    initialize_model();
    initialize_output_bitstream();
    initialize_arithmetic_encoder();
    for ( ; ; )
    {
    if ( ( ++text_count & 0x0ff ) == 0 )
            flush = check_compression();
        if ( !flush )
            c = getc( text_file );
        else
            c = FLUSH;
        if ( c == EOF )
            c = DONE;
        do {
            escaped = convert_int_to_symbol( c, &s );
            encode_symbol( compressed_file, &s );
        } while ( escaped );
        if ( c == DONE )
        break;
        if ( c == FLUSH )
    {
        flush_model();
            flush = 0;
    }
        update_model( c );
        add_character_to_model( c );
    }
    flush_arithmetic_encoder( compressed_file );
    flush_output_bitstream( compressed_file );
    print_compression();
    fputc( '\n', stderr );
    exit( 0 );
    return 0;
}

/*
 * This routine is called to print the current compression ratio.
 * It prints out the number of input bytes, the number of output bytes,
 * and the bits per byte compression ratio.   This is done both as a
 * pacifier and as a seat-of-the-pants diagnostice.  A better version
 * of this routine would also print the local compression ratio.
 */
void print_compression()
{
    long total_input_bytes;
    long total_output_bytes;

    total_input_bytes  =  ftell( text_file );
    total_output_bytes = bit_ftell_output( compressed_file );
    if ( total_output_bytes == 0 )
        total_output_bytes = 1;

    fprintf( stderr,"%ld/%ld, %2.3f\r",
         total_input_bytes,
         total_output_bytes,
             8.0 * total_output_bytes / total_input_bytes );
}

/*
 * This routine is called once every 256 input symbols.  Its job is to
 * check to see if the compression ratio hits or exceeds 90%.  If the
 * output size is 90% of the input size, it means not much compression
 * is taking place, so we probably ought to flush the statistics in the
 * model to allow for more current statistics to have greater impactic.
 * This heuristic approach does seem to have some effect.
 */
int check_compression()
{
    static long local_input_marker = 0L;
    static long local_output_marker = 0L;
    long total_input_bytes;
    long total_output_bytes;
    int local_ratio;

    print_compression();
    total_input_bytes  =  ftell( text_file ) - local_input_marker;
    total_output_bytes = bit_ftell_output( compressed_file );
    total_output_bytes -= local_output_marker;
    if ( total_output_bytes == 0 )
        total_output_bytes = 1;
    local_ratio = (int)( ( total_output_bytes * 100 ) / total_input_bytes );

    local_input_marker = ftell( text_file );
    local_output_marker = bit_ftell_output( compressed_file );

    if ( local_ratio > 90 && flushing_enabled )
    {
        fprintf( stderr, "Flushing... \r" );
        return( 1 );
    }
    return( 0 );
}


