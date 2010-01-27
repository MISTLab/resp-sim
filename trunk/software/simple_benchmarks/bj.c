#define UNIX 1

/************************************************/
/* Program: bj.c                                */
/* Version: 1.0                                 */
/* Date:    091345PST Mar 1997                  */
/* Name:    Alfred A. Aburto Jr.                */
/* Email:   aburto@nosc.mil, aburto@cts.com     */
/* Place:   San Diego, CA, USA                  */
/*                                              */
/* A program to evaluate playing and betting    */
/* strategies in the game of Blackjack.         */
/*                                              */
/* Example compilation using the "UNIX" or      */
/* "MSC" timer options and "gcc":               */
/* gcc -DUNIX -O3 bj.c -o bj, or for "MSC" as,  */
/* gcc -MSC -O3 bj.c -o bj                      */
/*                                              */
/* Of course one needs to use the right timer   */
/* timer (see options below) for your system    */
/* and whatever C compilers you may use...      */
/*                                              */
/* Run as: bj >> bj.res                         */
/*                                              */
/* NOTE: The programs input parameters are      */
/* defined in file "bj.in". Please do not       */
/* change those parameters for benchmarking.    */
/*                                              */
/************************************************/

/***************************************************************/
/* Timer options. You MUST uncomment one of the options below  */
/* or compile, for example, with the '-DUNIX' option.          */
/***************************************************************/
/* #define Amiga       */
/* #define UNIX        */
/* #define UNIX_Old    */
/* #define VMS         */
/* #define BORLAND_C   */
/* #define MSC         */
/* #define MAC         */
/* #define IPSC        */
/* #define FORTRAN_SEC */
/* #define GTODay      */
/* #define CTimer      */
/* #define UXPM        */
/* #define MAC_TMgr    */
/* #define PARIX       */
/* #define POSIX       */
/* #define WIN32       */
/* #define POSIX1      */
/***********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/****************************************/
/* Parameters for the mother of random  */
/* number generation programs: Mother() */
/****************************************/
#define m16Long   65536L        /* 2^16 */
#define m16Mask   0xFFFF        /* mask for lower 16 bits */
#define m15Mask   0x7FFF        /* mask for lower 15 bits */
#define m31Mask   0x7FFFFFFF    /* mask for 31 bits */
#define m32Double 4294967295.0  /* 2^32-1 */

static short mother1[10];
static short mother2[10];
static short mStart = 1;

#define NDecks    4
int card[52 * NDecks];
int   st[ 4 * NDecks];

int hards[22][12];
int softs[12][12];
int splts[12][12];
int stsw[12][12];
int stsl[12][12];
int stsp[12][12];
int sthw[22][12];
int sthl[22][12];
int sthp[22][12];
int stpw[12][12];
int stpl[12][12];
int stpp[12][12];
int sthb[22][12];
int stsb[12][12];
int sum[50];

/**************************************************/
/* For test purposes define the following:        */
/* IPrint = 1 ==> print out debugging info        */
/* IPrint = 0 ==> do not print out debugging info */
/* Be sure to reset nhands to a small number      */
/* when printing out results!!                    */
/**************************************************/
#define IPrint  0

int main()
{
 int  i, j, k, m, n, nhands = 0;   
 int  p, p_start, pmax, NCards, PShuf, shuffle();
 int  ddn, dup, pca, pcb, pcc, pcd, pc, pcs, pt, dt;
 int  itype, isoft, sh, shb, ia, ib, ic, id, pts;
 int  ihand, ihist, idbl, IStrat, IBet, sw, sl, sp;
 int  count, ca[12];

 double w, x, y, z;
 double starttime, benchtime;
 double base_bet, bet, win;
 double max_win, max_loss, player_advantage, avg_win_per_hand;

 FILE *fra;
 
 char file_rd[128];

 /*####################*/
 /*#  Initialization  #*/
 /*####################*/
 printf("#######################################\n");
 printf("Program: bj.c, 091345PST Mar 1997\n");
 printf("\n");
 printf("    Hands  Winnings\n");

 NCards = 52 * NDecks;
 PShuf  = 3;
 pmax   = NCards - 26;
 ihand  = 0;
 ihist  = 0;
 IStrat = 1;

 p        = 0;
 base_bet = 5.0;
 IBet     = 0;
 win      = 0.0;

 for (i = 0; i < 12; i++)
    {
     for (j = 0; j < 12; j++)
    {
     stsw[i][j] = 0;
     stsl[i][j] = 0;
     stsp[i][j] = 0;
     stpw[i][j] = 0;
     stpl[i][j] = 0;
     stpp[i][j] = 0;
     stsb[i][j] = 0;
    }
    }

 for (i = 0; i < 22; i++)
    {
     for (j = 0; j < 12; j++)
    {
     sthw[i][j] = 0;
     sthl[i][j] = 0;
     sthp[i][j] = 0;
     sthb[i][j] = 0;
    }
    }

 /*****************************/
 /*  Open input file 'bj.in'  */
 /*****************************/
 strcpy(file_rd,"software/benchmarks/data/bj.in");
 
 fra = fopen(file_rd,"r");
 
 if (fra == NULL)
   {
    printf("\n");
    printf("Error opening file: bj.in!\n");
    printf("See bj.c file for bj.in format!\n\n");
    exit(0);
   }

 /*****************************/
 /*  Get 'nhands' from bj.in  */
 /*  Default is 1000000 hands */
 /*****************************/
 fscanf(fra,"%d",&nhands);

 if (nhands < 1)
   {
    printf("\n");
    printf("Error: nhands too small enter larger value.\n");
    exit(0);
   }

 if (IPrint == 1)
   {
    nhands = 1; //Original value 300
    printf("\n");
    printf("IPrint = 1 so nhands preset to 10\n");
   }
 
 /**********************************/
 /* ca[i] holds the count strategy */
 /* Default is the 'Parker +/-'    */
 /**********************************/
 ca[0]  =  0;
 ca[1]  =  0;
 count  =  0;
 
 fscanf(fra,"%d %d %d %d %d",&ca[2],&ca[3],&ca[4],&ca[5],&ca[6]);
 fscanf(fra,"%d %d %d %d %d",&ca[7],&ca[8],&ca[9],&ca[10],&ca[11]);

 /*********************************************/
 /*  PSuf is the number of preshuffles done.  */
 /*  Default is PSuf = 3                      */
 /*********************************************/
 fscanf(fra,"%d",&PShuf);

 if (PShuf < 0)
   {
    printf("\n");
    printf("Error: PShuf too small! Enter larger value.\n");
    exit(0);
   }
 
 /*******************************************/
 /* This is the betting strategy option.    */
 /* IBet < 0 ==> is not allowed.            */
 /* IBet = 0 ==> use flat bet               */
 /* IBet = 1 ==> Bet according to count:    */
 /*******************************************/
 fscanf(fra,"%d",&IBet);

 if (IBet < 0)
   {
    printf("\n");
    printf("Error: IBet negative! Reset to IBet = 0\n");
    IBet = 0;
   }
 
 fscanf(fra,"%lf",&base_bet);

 if (base_bet <= 0.0)
   {
    printf("\n");
    printf("Error: base_bet negative. Reset to base_bet = 5.0\n");
    base_bet = 5.0;
   }
 
 fscanf(fra,"%d",&IStrat);

 if (IStrat < 0)
   {
    printf("\n");
    printf("Error: IStrat negative! Reset to IStrat = 1\n");
    IStrat = 1;
   }
 
 if (IStrat == 0) no_strat();
 if (IStrat == 1) basic_strat();

 fclose(fra);

 /********************************************/
 /* The following array saves parameters     */
 /* and statistics of interest:              */
 /* sum[0]  = number of player wins          */
 /* sum[1]  = number of player loses         */
 /* sum[2]  = number of player pushes        */
 /* sum[3]  = number of player busts         */
 /* sum[4]  = number of dealer busts         */
 /* sum[5]  = number of player blackjacks    */
 /* sum[6]  = number of dealer blackjacks    */
 /* sum[7]  = number of blackjack pushes     */
 /* sum[8]  = number of double  down    wins */
 /* sum[9]  = number of double  down   loses */
 /* sum[10] = number of double  down  pushes */
 /* sum[11] = number of initial split   wins */
 /* sum[12] = number of initial split  loses */
 /* sum[13] = number of initial split pushes */
 /* sum[14] = number of initial hard    wins */
 /* sum[15] = number of initial hard   loses */
 /* sum[16] = number of initial hard  pushes */
 /* sum[17] = number of initial soft    wins */
 /* sum[18] = number of initial soft   loses */
 /* sum[19] = number of initial soft  pushes */
 /********************************************/
 for (i = 0; i < 50; i++)
    {
     sum[i] = 0;
    }

 /****************************/
 /* Start timing the program */
 /****************************/

 /***********************/
 /* Preset the NDecks   */
 /* of cards:           */
 /*  1 is ACE           */
 /* 11 is JACK          */
 /* 12 is QUEEN         */
 /* 13 is KING          */
 /* Ignore the suit...  */
 /***********************/
 for (k = 0; k < NDecks; k++)
    {
     n = 52 * k;
     for (j = 0; j < 4; j++)
      {
       m = 13 * j + n;
       for (i = 1; i < 14; i++)
        {
         card[i + m - 1] = i;
        }
      }
    }
 
 /***************************/
 /* Convert to a Blackjack  */ 
 /* deck of cards where:    */
 /* JACK  ==> 10            */
 /* QUEEN ==> 10            */
 /* KING  ==> 10, and       */
 /* ACE   ==> 11            */      
 /***************************/
 for (i = 0; i < NCards; i++)
    {
     j = card[i];
     if ((j == 11) || (j == 12) || (j == 13)) card[i] = 10;
     if (j == 1) card[i] = 11;
    }
 
 /******************************/
 /* Shuffle the stack of cards */
 /* 3 times before starting.   */
 /******************************/
 for (i = 0; i < PShuf; i++)
    {
     shuffle();
    }
 
 /**************************************/
 /* Set some cards for debug purposes. */
 /* Uncomment and enter test cases and */
 /* recompile ....                     */
 /**************************************/

/*
 card[0]  =  8;
 card[1]  = 11;
 card[2]  =  9;
 card[3]  =  6;
 card[4]  =  8;
 card[5]  =  8;
 card[6]  =  9;
 card[7]  = 10;
 card[8]  = 11;
 card[9]  =  8;
 card[10] =  7;
 card[11] =  6;
 card[12] =  2;
 card[13] = 10;
 card[14] =  2;
*/

 /***************************************/
 /* Print out the cards if IPrint == 1  */
 /* Make sure nhands is not too big!    */
 /***************************************/
 if ( IPrint == 1)
   {
    for (i = 0; i < NCards; i++)
       {
    printf("%05d %5d\n",i,card[i]);
       }
   }

 /**********************************************/
 /* We are now ready to deal the cards. "p" is */ 
 /* the card counter. "pt" is the player total */
 /* and "dt" is the dealer total count. In the */
 /* initial round 4 cards are dealt.           */
 /* p   is the players first  card (pca)       */
 /* p+1 is the dealers hole   card (ddn)       */
 /* p+2 is the players second card (pcb)       */
 /* p+3 is the dealers up     card (dup)       */
 /**********************************************/
 start:
 
 /***************************************/
 /* Clear the split hand counting array */
 /***************************************/
 for (i = 0; i < (4 * NDecks); i++)
    {
     st[i] = 0;
    }
 sh = 0;

 /*****************************************************/
 /* First, before dealing more cards, check to see if */
 /* we need to shuffle the deck.                      */
 /*****************************************************/
 if ( p > pmax )
   {
    shuffle();
    p = 0;
    count = 0;

    /***********************************/
    /* Print out cards if IPrint == 1  */
    /* Probably should reset nhands    */
    /* to a reasonably small number!!  */
    /***********************************/
    if ( IPrint == 1)
      {
       for (i = 0; i < NCards; i++)
      {
       printf("%05d %5d\n",i,card[i]);
      }
      }
   }

 /**************************/
 /* Ok, place your bets :) */
 /**************************/
 bet = base_bet;
 
 if (IBet > 0)
   {
    if (count > 0) bet = (float)count * base_bet;
    if (count > 6) bet = 6.0 * base_bet;
   }

 /***********************************/
 /* Deal the initial 2 cards to the */
 /* player and dealer. First card   */
 /* to player, next to dealer, next */
 /* to player, then last card, the  */
 /* up card, to the dealer.         */
 /***********************************/
 pca = card[p];
 pcb = card[p+2];
 count = count + ca[pca];
 count = count + ca[pcb];

 ddn = card[p+1];
 dup = card[p+3];
 count = count + ca[ddn];
 count = count + ca[dup];

 dt  = ddn + dup;
 
 pt  = pca + pcb;
 pts = pt;

 p_start = p;

 p = p + 4;

 /********************************/
 /* Check for a player blackjack */
 /********************************/
 if ((dt != 21) && (pt == 21))
   {
    win = win + 1.5 * bet;
    sum[0]++;
    sum[5]++;
    itype = 2;
    goto tally;
   }

 /********************************/
 /* Check for a dealer blackjack */
 /********************************/
 if ((dt == 21) && (pt != 21))
   {
    win = win - bet;
    sum[1]++;
    sum[6]++;
    itype = 2;
    goto tally;
   }

 /*******************************************/
 /* Check for a dealer and player blackjack */
 /*******************************************/
 if ((dt == 21) && (pt == 21))
   {
    sum[2]++;
    sum[7]++;
    itype = 2;
    goto tally;
   }
 
 /**********************************************/
 /* At this point neither the dealer or player */
 /* has a blackjack. Check for other types of  */
 /* (initial) hands the player might have:     */
 /* hard  hand          ==> itype = 0          */
 /* soft  hand          ==> itype = 1          */
 /* blackjack           ==> itype = 2          */
 /* split hand          ==> itype = 3          */
 /**********************************************/
 itype = 0;
 if ((pca == 11) && (pcb != 11))
   {
    itype = 1;
    pcs   = pcb;
   }
 
 if ((pcb == 11) && (pca != 11))
   {
    itype = 1;
    pcs   = pca;
   }
 
 if (pca == pcb) itype = 3;

 /********************************/
 /* Check (initial) hard hands.  */
 /* Of course, anytime an ace is */
 /* dealt the hand becomes soft. */
 /********************************/
 if (itype == 0)
   {
    id = hards[pts][dup];
    
    /**********************/
    /* Check double down. */
    /* Allowed only with  */
    /* first two cards.   */
    /**********************/
    if (id == 2)
      {
       isoft = 1;
       bet   = 2.0 * bet;
       
       pcc = card[p];
       pt  = pt + pcc;
       p++;
       
       count = count + ca[pcc];
       
       if (pcc == 11) isoft++;

       if ((isoft > 0) && (pt > 21))
     {
      pt = pt - 10;
      isoft--;
     }
      } 
   
    /******************************************/
    /* Draw a card (hit) any number of times. */
    /******************************************/
    isoft = 0;
    if (id == 1)
      {
       do {
       pcc = card[p];
       pt  = pt + pcc;
       p++;
       
       count = count + ca[pcc];
       
       if (pcc == 11) isoft++;
       
       if ( isoft > 0) pc = pt - 10 * isoft - 1;

       if ((isoft > 0) && (pt > 21))
         {
          pt = pt - 10;
          isoft--;
         }
       ic  = 0;
       if ((isoft < 1) && (pt <= 21)) ic = hards[pt][dup];
       if ((isoft > 0) && (pt <= 21)) ic = softs[pc][dup];
      } while (ic != 0);
      }
    /*********/
    /* stand */
    /*********/
   }
 
 /***********************************/
 /* Check the (initial) soft hands. */
 /***********************************/
 if (itype == 1)
   {
    id = softs[pcs][dup];
    
    /***************/
    /* double down */
    /***************/
    if (id == 2)
      {
       isoft = 1;
       bet   = 2.0 * bet;
       
       pcc = card[p];
       pt  = pt + pcc;
       p++;
       
       count = count + ca[pcc];
       
       if (pcc == 11) isoft++;

       if ((isoft > 0) && (pt > 21))
     {
      pt = pt - 10;
      isoft--;
     }
      } 
   
    /******************************************/
    /* Draw a card (hit) any number of times. */
    /******************************************/
    if (id == 1)
      {
       isoft = 1;
       do {
       pcc = card[p];
       pt  = pt + pcc;
       p++;
       
       count = count + ca[pcc];
       
       if (pcc == 11) isoft++;
       
       if ( isoft > 0) pc = pt - 10 * isoft - 1;
       
       if ((isoft > 0) && (pt > 21))
         {
          pt = pt - 10;
          isoft--;
         }
       
       ic  = 0;
       if ((isoft < 1) && (pt <= 21)) ic = hards[pt][dup];
       if ((isoft > 0) && (pt <= 21)) ic = softs[pc][dup];
      } while (ic != 0);
      }
    /*********/
    /* stand */
    /*********/
   }
 
 /********************************/
 /* Check (initial) split hands. */
 /********************************/
 if (itype == 3)
   {
    sh = 0;
    id = splts[pca][dup];
    
    /********************************/
    /* Two (initial) 5's are 10 and */
    /* it is a double down case.    */
    /********************************/
    if (id == 2)
      {
       isoft = 1;
       bet   = 2.0 * bet;
       
       pcc = card[p];
       pt  = pt + pcc;
       p++;
       
       count = count + ca[pcc];
    
       if (pcc == 11) isoft++;

       if ((isoft > 0) && (pt > 21))
     {
      pt = pt - 10;
      isoft--;
     }
      } 
    
    /***************************************/
    /* If we stand, based on the strategy, */
    /* then we have a hard hand.           */
    /***************************************/

    /******************************************/
    /* Draw a card (hit) any number of times. */
    /******************************************/
    if (id == 1)
      {
       isoft = 0;
       do {
       pcc = card[p];
       pt  = pt + pcc;
       p++;
       
       count = count + ca[pcc];
       
       if (pcc == 11) isoft++;
       
       if ( isoft > 0) pc = pt - 10 * isoft - 1;

       if ((isoft > 0) && (pt > 21))
         {
          pt = pt - 10;
          isoft--;
         }
       ic  = 0;
       if ((isoft < 1) && (pt <= 21)) ic = hards[pt][dup];
       if ((isoft > 0) && (pt <= 21)) ic = softs[pc][dup];
      } while (ic != 0);
      }
    
    /****************************************/
    /* Split the cards any number of times. */
    /****************************************/
    if (id == 3)
      {
       /************************************/
       /* Note in our case here we do not  */
       /* allow double down after a split  */
       /* (some casinos do).               */
       /************************************/
       m  = 0;
       sh = 2;
       do {
       isoft = 0;
       st[m] = pca;
       if (pca == 11) isoft = 1;

       /*********************************/
       /* If the next card is the same  */
       /* as the split card (pca) then  */
       /* it is another split hand.     */
       /* Skip that card...             */
       /*********************************/
       k = 1;
       while (k != 0)
        {
         if (card[p] == pca)
           {
            sh++;
            p++;
           }
           else
           {
            k = 0;
           }
         }
       
       /***********************************/
       /* If we are splitting aces then   */
       /* we get only one hit...          */
       /***********************************/
       do {
           pcc   = card[p];
           st[m] = st[m] + pcc;
           p++;
           
           count = count + ca[pcc];
       
           if (pcc == 11) isoft++;

           if (isoft > 0) pc = st[m] - 10 * isoft - 1;

           if ((isoft > 0) && (st[m] > 21))
         {
          st[m] = st[m] - 10;
          isoft--;
         }
           ic  = 0;
           pt  = st[m];
           if ((isoft < 1) && (pt <= 21)) ic = hards[pt][dup];
           if ((isoft > 0) && (pt <= 21)) ic = softs[pc][dup];
           if (pca == 11) ic = 0;
          } while (ic != 0);
       m++;
      } while (m < sh);
      }

   }

 /**************************************/
 /* If the player busts in a non-split */
 /* hand then the dealer does not take */
 /* any more cards of course!          */
 /* Also, the dealer does not take any */
 /* more cards if all the players      */
 /* split hands are busted.            */
 /**************************************/
 if ((sh == 0) && (pt > 21)) goto tally;

 if (sh > 0)
   {
    shb = 0;
    for (i = 0; i < sh; i++)
       {
    if (st[i] > 21) shb++;
       }
    if (sh == shb) goto tally;
   }
 
 /*************************************/
 /* Player's hand has been evaluated. */
 /* Now check dealers hand.           */
 /*************************************/

 isoft = 0;
 if (ddn == 11) isoft++;
 if (dup == 11) isoft++;

 if ((isoft > 0) && (dt > 21))
   {
    dt = dt - 10;
    isoft--;
   }

 ic = 0;
 if (dt < 17) ic = 1;

 while (ic != 0)
      {
       pcd = card[p];
       dt  = dt + pcd;
       p++;
       
       count = count + ca[pcd];
       
       if (pcd == 11) isoft++;
       
       if ((isoft > 0) && (dt > 21))
     {
      dt = dt - 10;
      isoft--;
     }
     
       ic  = 0;
       if (dt < 17) ic = 1;
      }

 /****************************************/
 /* Both the player's and dealer's hands */
 /* have now been evaluated. Tally up    */
 /* the win, loss, and push statistics.  */
 /****************************************/
 tally:
 
 if (sh == 0) 
   {
    ihand++;
    ihist++;
   }

 if (sh >  0)
   {
    ihand = ihand + sh;
    ihist = ihist + sh;
   }

 /*********************/
 /* Print out results */
 /*********************/
 if (IPrint == 1)
   {
    printf ("%05d %5d %3d %3d %3d %3d %3d %3d ",ihand,p_start,pca,pcb,dup,pt,dt,sh);
   }

 /*********************************/
 /* Tally up hard and soft hands. */
 /*********************************/
 if (itype < 2)
   {
    /************************************/
    /* Player busts. This is the great  */
    /* disadvantage to the player where */
    /* he has the first chance to bust! */
    /************************************/
    if (pt > 21)
      {
       sum[1]++;
       sum[3]++;
       
       if (itype == 0)
     {
      sum[15]++;
      sthl[pts][dup]++;
      sthb[pts][dup]++;
     }

       if (itype == 1)
     {
      sum[18]++;
      stsl[pcs][dup]++;
      stsb[pcs][dup]++;
     }

       win = win - bet;
      }
    
    /************************/
    /* Player did not bust. */
    /************************/
    if (pt < 22) 
      {
       /***************/
       /* Player wins */
       /***************/
       if (pt > dt) 
     {
      sum[0]++;
      if (id == 2) sum[8]++;
      
      if (itype == 0)
        {
         sum[14]++;
         sthw[pts][dup]++;
        }

      if (itype == 1)
        {
         sum[17]++;
         stsw[pcs][dup]++;
        }
       
      win = win + bet;
     }
       
       /****************/
       /* Dealer busts */
       /****************/
       if (dt > 21)
     {
      sum[0]++;
      sum[4]++;
      if (id == 2) sum[8]++;
      
      if (itype == 0)
        {
         sum[14]++;
         sthw[pts][dup]++;
        }
      
      if (itype == 1)
        {
         sum[17]++;
         stsw[pcs][dup]++;
        }
      
      win = win + bet;
     }
       
       /********/
       /* Push */
       /********/
       if (pt == dt)
     {
      sum[2]++;
      if (id == 2) sum[10]++;
      
      if (itype == 0)
        {
         sum[16]++;
         sthp[pts][dup]++;
        }
      
      if (itype == 1)
        {
         sum[19]++;
         stsp[pcs][dup]++;
        }
     }

       /***************/
       /* Dealer wins */
       /***************/
       if ((dt < 22) && (pt < dt))
     {
      sum[1]++;
      if (id == 2) sum[9]++;
      
      if (itype == 0)
        {
         sum[15]++;
         sthl[pts][dup]++;
        }
      
      if (itype == 1)
        {
         sum[18]++;
         stsl[pcs][dup]++;
        }
      
      win = win - bet;
     }
      }
   } /* itype */

 /*************************************/
 /* Tally up initial split hands that */
 /* were double, hit, or stand (that  */
 /* is, not actually split).          */
 /*************************************/
 if ((itype == 3) && (sh == 0))
   {
    if (pt > 21)
      {
       sum[1]++;
       sum[3]++;
       sum[12]++;
       stpl[pca][dup]++;
       win = win - bet;
      }
    
    /************************/
    /* Player did not bust. */
    /************************/
    if (pt < 22) 
      {
       /***************/
       /* Player wins */
       /***************/
       if (pt > dt) 
     {
      sum[0]++;
      sum[11]++;
      stpw[pca][dup]++;
      if (id == 2) sum[8]++;
      win = win + bet;
     }
       
       /****************/
       /* Dealer busts */
       /****************/
       if (dt > 21)
     {
      sum[0]++;
      sum[4]++;
      sum[11]++;
      stpw[pca][dup]++;
      if (id == 2) sum[8]++;
      win = win + bet;
     }
       
       /********/
       /* Push */
       /********/
       if (pt == dt)
     {
      sum[2]++;
      sum[13]++;
      stpp[pca][dup]++;
      if (id == 2) sum[10]++;
     }

       /***************/
       /* Dealer wins */
       /***************/
       if ((dt < 22) && (pt < dt))
     {
      sum[1]++;
      sum[12]++;
      stpl[pca][dup]++;
      if (id == 2) sum[9]++;
      win = win - bet;
     }
      }
   }

 /********************************/
 /* Tally up actual split hands. */
 /********************************/
 if ((itype == 3) && (sh > 0))
   {
    for (i = 0; i < sh; i++)
       {
    j = st[i];
    /****************/
    /* Player busts */
    /****************/
    if (j > 21)
      {
       sum[1]++;
       sum[3]++;
       sum[12]++;
       stpl[pca][dup]++;
       win = win - bet;
      }
    /************************/
    /* Player did not bust. */
    /************************/
    if (j < 22) 
      {
       /***************/
       /* Player wins */
       /***************/
       if (j > dt) 
         {
          sum[0]++;
          sum[11]++;
          stpw[pca][dup]++;
          win = win + bet;
         }
       
        /****************/
        /* Dealer busts */
        /****************/
        if (dt > 21)
          {
           sum[0]++;
           sum[4]++;
           sum[11]++;
           stpw[pca][dup]++;
           win = win + bet;
          }
       
        /********/
        /* Push */
        /********/
        if (j == dt)
          {
           sum[2]++;
           sum[13]++;
           stpp[pca][dup]++;
          }

        /***************/
        /* Dealer wins */
        /***************/
        if ((dt < 22) && (j < dt))
          {
           sum[1]++;
           sum[12]++;
           stpl[pca][dup]++;
           win = win - bet;
          }
      }
       }
   }

 ia = sum[0] - sum[1];

 if (IPrint == 1)
   {
    printf("%5d %5d %5d %5d %5d %8.2f\n",sum[0],sum[1],sum[2],ia,count,win);
   }

 if (ihist >= (nhands / 50))
   {
    printf("%9d %9.2f\n",ihand,win);
    ihist = 0;
   }

 if (ihand >= nhands) goto end;

 goto start;

 end:

 idbl = sum[8] + sum[9] + sum[10];
 max_loss   = base_bet * (float)(ihand + idbl);
 player_advantage  = 100.0 * (win / max_loss);
 avg_win_per_hand  = win / (float)ihand;

 printf("\n");
 printf("Run Time (sec) = %9.3f\n",benchtime);
 
 printf("\n");
 printf("Preshuffles: %5d\n",PShuf);

 if (IBet > 0)
   {
    printf("\n");
    printf("The card count used for betting:\n");
    for (i = 2; i < 12; i++) printf("%3d ",i);
    printf("\n");
    for (i = 2; i < 12; i++) printf("%3d ",ca[i]);
    printf("\n");
   }

 printf("\n");
 printf("Player  base       bet ($): %10.2f\n",base_bet);
 printf("Player  total  winnings($): %10.2f\n",win);
 printf("Average win per    hand($): %10.2f\n",avg_win_per_hand);
 printf("Player (percent) advantage:  %9.3f\n",player_advantage);
 
 printf("\n");
 w = 0.01 * (float)(sum[0]+sum[1]+sum[2]);
 x = (float)sum[0] / w;
 y = (float)sum[1] / w;
 z = (float)sum[2] / w;
 printf("Number of hands     played:  %9d\n",ihand);
 printf("Number of hands        won:  %9d (%6.2f)\n",sum[0],x);
 printf("Number of hands       lost:  %9d (%6.2f)\n",sum[1],y);
 printf("Number of hands       push:  %9d (%6.2f)\n",sum[2],z);

 printf("\n");
 w = 0.01 * (float)(sum[5]+sum[6]+sum[7]);
 x = (float)sum[5] / w;
 y = (float)sum[6] / w;
 z = (float)sum[7] / w;
 printf("Number of Player   Blackjacks: %7d (%6.2f)\n",sum[5],x);
 printf("Number of Dealer   Blackjacks: %7d (%6.2f)\n",sum[6],y);
 printf("Number of Blackjack    Pushes: %7d (%6.2f)\n",sum[7],z);
 
 printf("\n");
 w = 0.01 * (float)(sum[14]+sum[15]+sum[16]);
 x = (float)sum[14] / w;
 y = (float)sum[15] / w;
 z = (float)sum[16] / w;
 printf("Number of initial hard    wins: %7d (%6.2f)\n",sum[14],x);
 printf("Number of initial hard   loses: %7d (%6.2f)\n",sum[15],y);
 printf("Number of initial hard  pushes: %7d (%6.2f)\n",sum[16],z);
 
 printf("\n");
 w = 0.01 * (float)(sum[17]+sum[18]+sum[19]);
 x = (float)sum[17] / w;
 y = (float)sum[18] / w;
 z = (float)sum[19] / w;
 printf("Number of initial soft    wins: %7d (%6.2f)\n",sum[17],x);
 printf("Number of initial soft   loses: %7d (%6.2f)\n",sum[18],y);
 printf("Number of initial soft  pushes: %7d (%6.2f)\n",sum[19],z);
 
 printf("\n");
 i = sum[11];
 j = sum[12];
 k = sum[13];
 w = 0.01 * (float)(i + j + k);
 x = (float)i / w;
 y = (float)j / w;
 z = (float)k / w;
 printf("Number of initial split   wins: %7d (%6.2f)\n",i,x);
 printf("Number of initial split  loses: %7d (%6.2f)\n",j,y);
 printf("Number of initial split pushes: %7d (%6.2f)\n",k,z);
 
 printf("\n");
 w = 0.01 * (float)(sum[8]+sum[9]+sum[10]);
 x = (float)sum[8] / w;
 y = (float)sum[9] / w;
 z = (float)sum[10] / w;
 printf("Number of double down    wins: %7d (%6.2f)\n",sum[8],x);
 printf("Number of double down   loses: %7d (%6.2f)\n",sum[9],y);
 printf("Number of double down  pushes: %7d (%6.2f)\n",sum[10],z);
 
 printf("\n");
 printf("Number of hands player busted: %7d\n",sum[3]);
 printf("Number of hands dealer busted: %7d\n",sum[4]);

 printf("\n");
 printf("Initial 2 card hard hands: percentage of hands won.\n"); 
 printf("          Dealer up card:\n");
 printf("(player)  2      3      4      5      6      7      8");
 printf("      9     10     11\n");
 for (i = 5; i < 20; i++)
    {
     printf("%3d ",i);
     for (j = 2; j < 12; j++)
    {
     k = sthw[i][j] + sthl[i][j] + sthp[i][j];
     x = 0.0;
     if (k != 0) x = 100.0 * (float)sthw[i][j] / (float)k;
     printf("%7.1f",x);
    }
     printf("\n");
    }

 printf("\n");
 printf("Initial 2 card soft hands: percentage of hands won.\n"); 
 printf("          Dealer up card:\n");
 printf("(player)  2      3      4      5      6      7      8");
 printf("      9     10     11\n");
 for (i = 2; i < 10; i++)
    {
     printf(" A%1d ",i);
     for (j = 2; j < 12; j++)
    {
     k = stsw[i][j] + stsl[i][j] + stsp[i][j];
     x = 0.0;
     if (k != 0) x = 100.0 * (float)stsw[i][j] / (float)k;
     printf("%7.1f",x);
    }
     printf("\n");
    }
 
 printf("\n");
 printf("Initial 2 card split hands: percentage of hands won.\n"); 
 printf("          Dealer up card:\n");
 printf("(player)  2      3      4      5      6      7      8");
 printf("      9     10     11\n");
 for (i = 2; i < 12; i++)
    {
     printf("%2d%2d",i,i);
     for (j = 2; j < 12; j++)
    {
     k = stpw[i][j] + stpl[i][j] + stpp[i][j];
     x = 0.0;
     if (k != 0) x = 100.0 * (float)stpw[i][j] / (float)k;
     printf("%7.2f",x);
    }
     printf("\n");
    }
 
 printf("\n");
 printf("Initial 2 card hard hands: percentage of hands player busted.\n"); 
 printf("          Dealer up card:\n");
 printf("(player)  2      3      4      5      6      7      8");
 printf("      9     10     11\n");
 for (i = 5; i < 20; i++)
    {
     printf("%3d ",i);
     for (j = 2; j < 12; j++)
    {
     k = sthw[i][j] + sthl[i][j] + sthp[i][j];
     x = 0.0;
     if (k != 0) x = 100.0 * (float)sthb[i][j] / (float)k;
     printf("%7.1f",x);
    }
     printf("\n");
    }
 
 printf("\n");
 printf("Initial 2 card soft hands: percentage of hands player busted\n"); 
 printf("          Dealer up card:\n");
 printf("(player)  2      3      4      5      6      7      8");
 printf("      9     10     11\n");
 for (i = 2; i < 10; i++)
    {
     printf(" A%1d ",i);
     for (j = 2; j < 12; j++)
    {
     k = stsw[i][j] + stsl[i][j] + stsp[i][j];
     x = 0.0;
     if (k != 0) x = 100.0 * (float)stsb[i][j] / (float)k;
     printf("%7.1f",x);
    }
     printf("\n");
    }
 
 exit(0);
}

/***************************/
/* Card shuffling program. */
/***************************/
int shuffle()
{
 int i, j, k, l, m, n;
 unsigned long kr = 3141592;
 double r, x, scale, Mother();
 
 /**************************************/
 /*  If you want to test rand() you    */
 /*  will need to set the value of     */
 /*  scale as follows:                 */
 /*  scale  = 1.0 / (double)RAND_MAX;  */
 /*                                    */
 /*  For Mother() however scale = 1.0; */
 /**************************************/
 scale = 1.0;

 j = 52 * NDecks;
 r = (double)j * scale;
 k = 10 * j;
 for (i = 1; i <= k; i++)
    {
     m = (int)(r * Mother(&kr));
     n = (int)(r * Mother(&kr));
     /********************************/
     /*  m = (int)(r * rand());  */
     /*  n = (int)(r * rand());  */
     /********************************/
     l = card[n];
     card[n] = card[m];
     card[m] = l;
    }

 return 0;
}

/****************************************/
/* This routine sets the Basic Strategy */
/* that will be used. It is based upon  */
/* Lawrence Revere's Basic Strategy for */
/* four or more decks in the book:      */
/* "Playing Blackjack as a Business".   */
/* In all cases:                        */
/* 0 == stand                           */
/* 1 == hit                             */
/* 2 == double down                     */
/* 3 == split                           */
/****************************************/
int basic_strat()
{
 int i, j;

 /**************/
 /* Hard hands */
 /**************/
 for (i = 0; i < 22; i++)
    {
     for (j = 0; j < 12; j++)
    {
     hards[i][j] = 1;
     if (i > 16) hards[i][j] = 0;
    }
    }
 
 for (j = 3; j < 7; j++)
    {
     hards[9][j] = 2;
    }
 
 for (j = 2; j < 10; j++)
    {
     hards[10][j] = 2;
    }
 
 for (j = 2; j < 11; j++)
    {
     hards[11][j] = 2;
    }

 for (j = 4; j < 7; j++)
    {
     hards[12][j] = 0;
    }

 for (i = 13; i < 17; i++)
    {
     for (j = 2; j < 7;j++)
    {
     hards[i][j] = 0;
    }
    }
 
 /**************/
 /* Soft hands */
 /**************/
 for (i = 0; i < 12; i++)
    {
     for (j = 0; j < 12; j++)
    {
     softs[i][j] = 1;
     if (i > 7) softs[i][j] = 0;
    }
    }
 
 for (i = 2; i < 4; i++)
    {
     for (j = 5; j < 7;j++)
    {
     softs[i][j] = 2;
    }
    }
 
 for (i = 4; i < 6; i++)
    {
     for (j = 4; j < 7;j++)
    {
     softs[i][j] = 2;
    }
    }
 
 for (i = 6; i < 8; i++)
    {
     for (j = 3; j < 7;j++)
    {
     softs[i][j] = 2;
    }
    }
 
 softs[7][2] = 0;
 softs[7][7] = 0;
 softs[7][8] = 0;
 
 softs[4][3] = 2;
 softs[5][3] = 2;

 /***************/
 /* Split hands */
 /***************/
 for (i = 0; i < 12; i++)
    {
     for (j = 0; j < 12; j++)
    {
     splts[i][j] = 1;
    }
    }
 
 for (j = 0; j < 12; j++)
    {
     splts[8][j]  = 3;
     splts[9][j]  = 3;
     splts[10][j] = 0;
     splts[11][j] = 3;
    }
 
 splts[9][7]  = 0;
 splts[9][10] = 0;
 splts[9][11] = 0;

 for (i = 2; i < 4; i++)
    {
     for (j = 4; j < 8;j++)
    {
     splts[i][j] = 3;
    }
    }

 for (j = 3; j < 7; j++)
    {
     splts[6][j] = 3;
    }

 for (j = 2; j < 8; j++)
    {
     splts[7][j] = 3;
    }
 
 for (j = 0; j < 10; j++)
    {
     splts[5][j] = 2;
    }
 splts[5][10] = 1;
 splts[5][11] = 1;
}

/**********************************/
/* This is the simplest strategy  */
/* Because basically you just hit */
/* till you reach 17 or bust. As  */
/* one finds out, you can lose    */
/* alot this way. Aces are split  */
/* otherwise there is no splitting*/
/* or doubling down!              */
/**********************************/
int no_strat()
{
 int i, j;

 /**************/
 /* Hard hands */
 /**************/
 for (i = 0; i < 22; i++)
    {
     for (j = 0; j < 12; j++)
    {
     hards[i][j] = 1;
     if (i > 16) hards[i][j] = 0;
    }
    }
 
 /**************/
 /* Soft hands */
 /**************/
 for (i = 0; i < 12; i++)
    {
     for (j = 0; j < 12; j++)
    {
     softs[i][j] = 1;
     if (i > 7) softs[i][j] = 0;
    }
    }
 
 softs[7][2] = 0;
 softs[7][7] = 0;
 softs[7][8] = 0;
 
 /***************/
 /* Split hands */
 /***************/
 for (i = 0; i < 12; i++)
    {
     for (j = 0; j < 12; j++)
    {
     splts[i][j] = 1;
    }
    }
 
 for (j = 0; j < 12; j++)
    {
     splts[10][j] = 0;
     splts[11][j] = 3;
    }
}

/* Mother *************************************************************
|  George Marsaglia's The mother of all random number generators
|   producing uniformly distributed pseudo random 32 bit values
|   with period about 2^250.
|  The text of Marsaglia's posting is provided in the file mother.txt
|
|  The arrays mother1 and mother2 store carry values in their
|   first element, and random 16 bit numbers in elements 1 to 8.
|   These random numbers are moved to elements 2 to 9 and a new
|   carry and number are generated and placed in elements 0 and 1.
|  The arrays mother1 and mother2 are filled with random 16 bit
|   values on first call of Mother by another generator.  mStart
|   is the switch.
|
|  Returns:
|   A 32 bit random number is obtained by combining the output of the
|   two generators and returned in *pSeed.  It is also scaled by
|   2^32-1 and returned as a double between 0 and 1
|
|  SEED:
|   The inital value of *pSeed may be any long value
|
|   Bob Wheeler 8/8/94
*/

double Mother(unsigned long *pSeed)
{
 unsigned long  number, number1, number2;
 short n, *p;
 unsigned short sNumber;

 /* Initialize motheri with 9 random values the first time */
 if (mStart) 
   {
    sNumber= *pSeed&m16Mask;   /* The low 16 bits */
    number = *pSeed&m31Mask;   /* Only want 31 bits */

    p = mother1;
    for (n=18;n--;)
     {
      number=30903*sNumber+(number>>16);   /* One line multiply-with-cary */
      *p++=sNumber=number&m16Mask;
      if (n==9) p = mother2;
     }
    
    /* make cary 15 bits */
    mother1[0]&=m15Mask;
    mother2[0]&=m15Mask;
    mStart=0;
   }

 /* Move elements 1 to 8 to 2 to 9 */
 memmove(mother1+2,mother1+1,8*sizeof(short));
 memmove(mother2+2,mother2+1,8*sizeof(short));

 /* Put the carry values in numberi */
 number1=mother1[0];
 number2=mother2[0];

 /* Form the linear combinations */
 number1+=1941*mother1[2]+1860*mother1[3]+1812*mother1[4]+
 1776*mother1[5]+1492*mother1[6]+1215*mother1[7]+
 1066*mother1[8]+12013*mother1[9];
    
 number2+=1111*mother2[2]+2222*mother2[3]+3333*mother2[4]+
 4444*mother2[5]+5555*mother2[6]+6666*mother2[7]+
 7777*mother2[8]+9272*mother2[9];

 /* Save the high bits of numberi as the new carry */
 mother1[0]=number1/m16Long;
 mother2[0]=number2/m16Long;
        
 /* Put the low bits of numberi into motheri[1] */
 mother1[1]=m16Mask&number1;
 mother2[1]=m16Mask&number2;

 /* Combine the two 16 bit random numbers into one 32 bit */
 *pSeed=(((long)mother1[1])<<16)+(long)mother2[1];

 /* Return a double value between 0 and 1 */
 return ((double)*pSeed)/m32Double;
}

