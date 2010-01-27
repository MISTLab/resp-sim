#include <stdio.h>
#include <string.h>

#define	MIN_WORD	((-32767)-1)
#define	MAX_WORD	( 32767)
#define	MIN_LONGWORD	((-2147483647)-1)
#define	MAX_LONGWORD	( 2147483647)
#define	GSM_MAGIC	0xD			  	/* 13 kbit/s RPE-LTP */
#define	SASR(x, by)	((x) >> (by))

typedef short		  word;		/* 16 bit signed int	*/
typedef long		  longword;	/* 32 bit signed int	*/
typedef unsigned short    uword;		/* unsigned word	*/
typedef unsigned long     ulongword;	/* unsigned longword	*/

#define	SASR(x, by)	((x) >> (by))


word	gsm_mult 	(word a, word b);
longword gsm_L_mult (word a, word b);
word	gsm_mult_r	(word a, word b);
word	gsm_div  	(word num, word denum);
word	gsm_add 	( word a, word b);
longword gsm_L_add 	( longword a, longword b );
word	gsm_sub 	(word a, word b);
longword gsm_L_sub 	(longword a, longword b);
word	gsm_abs 	(word a);
word	gsm_norm 	( longword a);
longword gsm_L_asl  (longword a, int n);
word	gsm_asl 	(word a, int n);
longword gsm_L_asr  (longword a, int n);
word	gsm_asr  	(word a, int n);

void Gsm_gen(word * LARc, word * Nc, word * bc, word * Mc, word * xmaxc, word * xmc,unsigned char * c);

void Gsm_Preprocess(word *S_z1, longword *S_L_z2, word * S_mp,word * s,word * so);

void Autocorrelation (word     * s,longword * L_ACF);
void Reflection_coefficients (longword	* L_ACF,register word	* r);
void Transformation_to_Log_Area_Ratios (register word	* r );
void Quantization_and_coding (register word * LAR);
void Decoding_of_the_coded_Log_Area_Ratios (word * LARc,word	* LARpp);
void Coefficients_0_12 (register word * LARpp_j_1,register word * LARpp_j,register word * LARp);
void Coefficients_13_26 (register word * LARpp_j_1,register word * LARpp_j,register word * LARp);
void Coefficients_27_39 (register word * LARpp_j_1,register word * LARpp_j,register word * LARp);
void Coefficients_40_159 (register word * LARpp_j,register word * LARp);
void LARp_to_rp (register word * LARp);


void Short_term_analysis_filtering (word * S_u,register word	* rp,register int k_n, register word	* s);


void Search_for_the_maximum_cross_correlation(word * d, word scal,register word * dp,word * Nc,longword * L_max);
word Search_of_the_optimum_scaling(register word * d);
void Rescaling_of_L_max(longword * L_max,word scal);
void Compute_the_power_of_the_reconstructed_short_term(register word * dp,word * Nc_out,longword * L_power);
void Calculation_of_the_LTP_parameters (register word * d, register word * dp, word * bc_out, word * Nc_out);
void Long_term_analysis_filtering (word bc, word Nc, register word * dp, register word * d, register word * dpp, register word * e);



void Weighting_filter (register word * e, word * x);
void RPE_grid_selection (word * x, word * xM, word * Mc_out);
void APCM_quantization_xmaxc_to_exp_mant (word xmaxc, word * exp_out, word * mant_out );
void APCM_quantization (word * xM, word * xMc, word * mant_out,word * exp_out,word * xmaxc_out);
void APCM_inverse_quantization (register word	* xMc,word mant,word exp, register word	* xMp);
void RPE_grid_positioning (word Mc, register word * xMp, register word * ep);
void Increment(word * dp,word *dpp, word * e);

void Dump(word * S_LARpp0,word *S_LARpp1,word *S_dp0);
void Read_file(char * filein, word *Data);
void Write_file(char * fileout, unsigned char * c);

#define NUMFRAME 9

////*****FUNCTIONS*****************************************

void Gsm_gen(word * LARc, word * Nc, word * bc, word * Mc, word * xmaxc, word * xmc,unsigned char * c)
{
	*c++ =   ((GSM_MAGIC & 0xF) << 4)		/* 1 */
	       | ((LARc[0] >> 2) & 0xF);
	*c++ =   ((LARc[0] & 0x3) << 6)
	       | (LARc[1] & 0x3F);
	*c++ =   ((LARc[2] & 0x1F) << 3)
	       | ((LARc[3] >> 2) & 0x7);
	*c++ =   ((LARc[3] & 0x3) << 6)
	       | ((LARc[4] & 0xF) << 2)
	       | ((LARc[5] >> 2) & 0x3);
	*c++ =   ((LARc[5] & 0x3) << 6)
	       | ((LARc[6] & 0x7) << 3)
	       | (LARc[7] & 0x7);
	*c++ =   ((Nc[0] & 0x7F) << 1)
	       | ((bc[0] >> 1) & 0x1);
	*c++ =   ((bc[0] & 0x1) << 7)
	       | ((Mc[0] & 0x3) << 5)
	       | ((xmaxc[0] >> 1) & 0x1F);
	*c++ =   ((xmaxc[0] & 0x1) << 7)
	       | ((xmc[0] & 0x7) << 4)
	       | ((xmc[1] & 0x7) << 1)
	       | ((xmc[2] >> 2) & 0x1);
	*c++ =   ((xmc[2] & 0x3) << 6)
	       | ((xmc[3] & 0x7) << 3)
	       | (xmc[4] & 0x7);
	*c++ =   ((xmc[5] & 0x7) << 5)			/* 10 */
	       | ((xmc[6] & 0x7) << 2)
	       | ((xmc[7] >> 1) & 0x3);
	*c++ =   ((xmc[7] & 0x1) << 7)
	       | ((xmc[8] & 0x7) << 4)
	       | ((xmc[9] & 0x7) << 1)
	       | ((xmc[10] >> 2) & 0x1);
	*c++ =   ((xmc[10] & 0x3) << 6)
	       | ((xmc[11] & 0x7) << 3)
	       | (xmc[12] & 0x7);
	*c++ =   ((Nc[1] & 0x7F) << 1)
	       | ((bc[1] >> 1) & 0x1);
	*c++ =   ((bc[1] & 0x1) << 7)
	       | ((Mc[1] & 0x3) << 5)
	       | ((xmaxc[1] >> 1) & 0x1F);
	*c++ =   ((xmaxc[1] & 0x1) << 7)
	       | ((xmc[13] & 0x7) << 4)
	       | ((xmc[14] & 0x7) << 1)
	       | ((xmc[15] >> 2) & 0x1);
	*c++ =   ((xmc[15] & 0x3) << 6)
	       | ((xmc[16] & 0x7) << 3)
	       | (xmc[17] & 0x7);
	*c++ =   ((xmc[18] & 0x7) << 5)
	       | ((xmc[19] & 0x7) << 2)
	       | ((xmc[20] >> 1) & 0x3);
	*c++ =   ((xmc[20] & 0x1) << 7)
	       | ((xmc[21] & 0x7) << 4)
	       | ((xmc[22] & 0x7) << 1)
	       | ((xmc[23] >> 2) & 0x1);
	*c++ =   ((xmc[23] & 0x3) << 6)
	       | ((xmc[24] & 0x7) << 3)
	       | (xmc[25] & 0x7);
	*c++ =   ((Nc[2] & 0x7F) << 1)			/* 20 */
	       | ((bc[2] >> 1) & 0x1);
	*c++ =   ((bc[2] & 0x1) << 7)
	       | ((Mc[2] & 0x3) << 5)
	       | ((xmaxc[2] >> 1) & 0x1F);
	*c++ =   ((xmaxc[2] & 0x1) << 7)
	       | ((xmc[26] & 0x7) << 4)
	       | ((xmc[27] & 0x7) << 1)
	       | ((xmc[28] >> 2) & 0x1);
	*c++ =   ((xmc[28] & 0x3) << 6)
	       | ((xmc[29] & 0x7) << 3)
	       | (xmc[30] & 0x7);
	*c++ =   ((xmc[31] & 0x7) << 5)
	       | ((xmc[32] & 0x7) << 2)
	       | ((xmc[33] >> 1) & 0x3);
	*c++ =   ((xmc[33] & 0x1) << 7)
	       | ((xmc[34] & 0x7) << 4)
	       | ((xmc[35] & 0x7) << 1)
	       | ((xmc[36] >> 2) & 0x1);
	*c++ =   ((xmc[36] & 0x3) << 6)
	       | ((xmc[37] & 0x7) << 3)
	       | (xmc[38] & 0x7);
	*c++ =   ((Nc[3] & 0x7F) << 1)
	       | ((bc[3] >> 1) & 0x1);
	*c++ =   ((bc[3] & 0x1) << 7)
	       | ((Mc[3] & 0x3) << 5)
	       | ((xmaxc[3] >> 1) & 0x1F);
	*c++ =   ((xmaxc[3] & 0x1) << 7)
	       | ((xmc[39] & 0x7) << 4)
	       | ((xmc[40] & 0x7) << 1)
	       | ((xmc[41] >> 2) & 0x1);
	*c++ =   ((xmc[41] & 0x3) << 6)			/* 30 */
	       | ((xmc[42] & 0x7) << 3)
	       | (xmc[43] & 0x7);
	*c++ =   ((xmc[44] & 0x7) << 5)
	       | ((xmc[45] & 0x7) << 2)
	       | ((xmc[46] >> 1) & 0x3);
	*c++ =   ((xmc[46] & 0x1) << 7)
	       | ((xmc[47] & 0x7) << 4)
	       | ((xmc[48] & 0x7) << 1)
	       | ((xmc[49] >> 2) & 0x1);
	*c++ =   ((xmc[49] & 0x3) << 6)
	       | ((xmc[50] & 0x7) << 3)
	       | (xmc[51] & 0x7);
}


void Increment(word * dp,word *dpp, word * e){
  int i;
  for (i = 0; i <= 39; i++)
    dp[ i ] = gsm_add( e[5 + i], dpp[i] );
}

//void Gsm_Preprocess (word *S_z1, longword *S_L_z2, word * S_mp,word * s,word * so )
void Gsm_Preprocess (word *S_z1, longword *S_L_z2, word * S_mp,word * s,word * so )
{
        word 	   s1;
	longword   L_s2;
	longword   L_temp;
	word	   msp, lsp, temp;
	word	   SO,sof;

	register int  k;
//	int i;
	
	for (k=0 ; k <= 159 ; k++) {
		SO = SASR( s[k] , 3 ) << 2;
		s1 = SO - *S_z1;
		*S_z1 = SO;
		L_s2 = s1;
		L_s2 <<= 15;
		msp = *S_L_z2 >> 15;
		lsp = *S_L_z2 - (msp <<15);
		temp = gsm_mult_r( lsp, 32735 );
		L_s2 = gsm_L_add (L_s2, temp);
		L_temp = gsm_L_mult(msp,  32735) >> 1;
		*S_L_z2   = gsm_L_add(L_s2, L_temp);
		sof = gsm_L_add( *S_L_z2, 16384 ) >> 15;
		msp   = gsm_mult_r( *S_mp, -28180 );
		so[k] = gsm_add( sof, msp );
		*S_mp = sof;
	}
        
}

void Autocorrelation (word     * s,longword * L_ACF)
{
	register int	k, i;
	word		temp, smax, scalauto;
	smax = 0;
	for (k = 0; k <= 159; k++) {
		temp = gsm_abs( s[k] );
		if (temp > smax) smax = temp;
	}
	if (smax == 0) scalauto = 0;
	else {
		scalauto = 4 - gsm_norm( (longword)smax << 16 );/* sub(4,..) */
	}
	if (scalauto > 0) {

#   define SCALE(n) case n: for (k = 0; k <= 159; k++) s[k] = gsm_mult_r( s[k], 16384 >> (n-1) ); break;

		switch (scalauto) {
		SCALE(1)
		SCALE(2)
		SCALE(3)
		SCALE(4)
		}
# undef	SCALE
	}
	{
	word  * sp = s;
	word    sl = *sp;

#	define STEP(k)	 L_ACF[k] += (longword)((longword)(sl) * (longword)(sp[ -(k) ]));
#	define NEXTI	 sl = *++sp


	for (k = 9; k--; L_ACF[k] = 0) ;

	STEP (0);
	NEXTI;
	STEP(0); STEP(1);
	NEXTI;
	STEP(0); STEP(1); STEP(2);
	NEXTI;
	STEP(0); STEP(1); STEP(2); STEP(3);
	NEXTI;
	STEP(0); STEP(1); STEP(2); STEP(3); STEP(4);
	NEXTI;
	STEP(0); STEP(1); STEP(2); STEP(3); STEP(4); STEP(5);
	NEXTI;
	STEP(0); STEP(1); STEP(2); STEP(3); STEP(4); STEP(5); STEP(6);
	NEXTI;
	STEP(0); STEP(1); STEP(2); STEP(3); STEP(4); STEP(5); STEP(6); STEP(7);

	for (i = 8; i <= 159; i++) {

		NEXTI;

		STEP(0);
		STEP(1); STEP(2); STEP(3); STEP(4);
		STEP(5); STEP(6); STEP(7); STEP(8);
	}

	for (k = 9; k--; L_ACF[k] <<= 1) ;

	}
	if (scalauto > 0) {
		for (k = 160; k--; *s++ <<= scalauto) ;
	}

}


void Reflection_coefficients (longword	* L_ACF,register word	* r)
{
	register int	i, m, n;
	register word	temp;
	word		ACF[9];	/* 0..8 */
	word		P[  9];	/* 0..8 */
	word		K[  9]; /* 2..8 */


	if (L_ACF[0] == 0) {	/* everything is the same. */
		return;
	}

	temp = gsm_norm( L_ACF[0] );

	for (i = 0; i <= 8; i++) ACF[i] = SASR( L_ACF[i] << temp, 16 );
	for (i = 1; i <= 7; i++) K[ i ] = ACF[ i ];
	for (i = 0; i <= 8; i++) P[ i ] = ACF[ i ];
	for (n = 1; n <= 8; n++, r++) {

		temp = P[1];
		temp = gsm_abs(temp);
		if (P[0] < temp) {
			break;
		}

		*r = gsm_div( temp, P[0] );
		if (P[1] > 0) *r = -*r;		/* r[n] = sub(0, r[n]) */
		if (n == 8){ 
			break;
    }
		temp = gsm_mult_r( P[1], *r );
		P[0] = gsm_add( P[0], temp );

		for (m = 1; m <= 8 - n; m++) {
			temp     = gsm_mult_r( K[ m   ],    *r );
			P[m]     = gsm_add(    P[ m+1 ],  temp );
			temp     = gsm_mult_r( P[ m+1 ],    *r );
			K[m]     = gsm_add(    K[ m   ],  temp );
		}
	}
}

void Transformation_to_Log_Area_Ratios (register word	* r )
{
	register word	temp;
	register int	i;
	for (i = 1; i <= 8; i++, r++) {
		temp = *r;
		temp = gsm_abs(temp);
		if (temp < 22118) {
			temp >>= 1;
		} else if (temp < 31130) {
			temp -= 11059;
		} else {
			temp -= 26112;
			temp <<= 2;
		}
		*r = *r < 0 ? -temp : temp;
	}
}

void Quantization_and_coding (register word * LAR)
{
	register word	temp;
//  int i;
#	undef STEP
#	define	STEP( A, B, MAC, MIC ) temp = gsm_mult( A,   *LAR ); temp = gsm_add(	temp,   B ); temp = gsm_add(  temp, 256 ); temp = SASR(     temp,   9 );	*LAR  =  temp>MAC ? MAC - MIC : (temp<MIC ? 0 : temp - MIC);	printf("Quantization_and_coding LARC[u]=%d\n",*LAR); LAR++;
	STEP(  20480,     0,  31, -32 );
	STEP(  20480,     0,  31, -32 );
	STEP(  20480,  2048,  15, -16 );
	STEP(  20480, -2560,  15, -16 );

	STEP(  13964,    94,   7,  -8 );
	STEP(  15360, -1792,   7,  -8 );
	STEP(   8534,  -341,   3,  -4 );
	STEP(   9036, -1144,   3,  -4 );

#	undef	STEP

}


void Decoding_of_the_coded_Log_Area_Ratios (word * LARc,word	* LARpp)
{
	register word	temp1;
//	int i;

#undef	STEP
#define	STEP( B, MIC, INVA ) temp1    = gsm_add( *LARc++, MIC ) << 10; temp1	= gsm_sub( temp1, B << 1 ); temp1    = gsm_mult_r( INVA, temp1 ); *LARpp = gsm_add( temp1, temp1 ); printf("Decoding_of_the_coded_Log_Area_Ratios LARpp[u]=%d\n",*LARpp); LARpp++;

	STEP(      0,  -32,  13107 );
	STEP(      0,  -32,  13107 );
	STEP(   2048,  -16,  13107 );
	STEP(  -2560,  -16,  13107 );

	STEP(     94,   -8,  19223 );
	STEP(  -1792,   -8,  17476 );
	STEP(   -341,   -4,  31454 );
	STEP(  -1144,   -4,  29708 );
	
}

void Coefficients_0_12 (register word * LARpp_j_1,register word * LARpp_j,register word * LARp)
{
	register int 	i;
	for (i = 1; i <= 8; i++, LARp++, LARpp_j_1++, LARpp_j++) {
		*LARp = gsm_add( SASR( *LARpp_j_1, 2 ), SASR( *LARpp_j, 2 ));
		*LARp = gsm_add( *LARp,  SASR( *LARpp_j_1, 1));
	}
}

void Coefficients_13_26 (register word * LARpp_j_1,register word * LARpp_j,register word * LARp)
{
	register int i;
	for (i = 1; i <= 8; i++, LARpp_j_1++, LARpp_j++, LARp++) {
		*LARp = gsm_add( SASR( *LARpp_j_1, 1), SASR( *LARpp_j, 1 ));
	}
}

void Coefficients_27_39 (register word * LARpp_j_1,register word * LARpp_j,register word * LARp)
{
	register int i;
	for (i = 1; i <= 8; i++, LARpp_j_1++, LARpp_j++, LARp++) {
		*LARp = gsm_add( SASR( *LARpp_j_1, 2 ), SASR( *LARpp_j, 2 ));
		*LARp = gsm_add( *LARp, SASR( *LARpp_j, 1 ));
	}
}


void Coefficients_40_159 (register word * LARpp_j,register word * LARp)
{
	register int i;
	for (i = 1; i <= 8; i++, LARp++, LARpp_j++){
		*LARp = *LARpp_j;
	}
}

void LARp_to_rp (register word * LARp)
{
	register int 		i;
	register word		temp;
	for (i = 1; i <= 8; i++, LARp++) {
		if (*LARp < 0) {
			temp = *LARp == MIN_WORD ? MAX_WORD : -(*LARp);
			*LARp = - ((temp < 11059) ? temp << 1
				: ((temp < 20070) ? temp + 11059
				:  gsm_add( temp >> 2, 26112 )));
		} else {
			temp  = *LARp;
			*LARp =    (temp < 11059) ? temp << 1
				: ((temp < 20070) ? temp + 11059
				:  gsm_add( temp >> 2, 26112 ));
		}
	}
}

void Short_term_analysis_filtering (word *S_u, register word	* rp,register int k_n, register word	* s)
{

  word k,i;
  word di,sav,temp,ui;

  for ( k = 0 ; k < k_n ; k++)	{
    di = sav = s[k];
    for ( i = 0 ; i < 8 ; i++)	{
      ui = S_u[i];
      S_u[i] = sav;

      if(rp[i] == MIN_WORD && di == MIN_WORD) temp = MAX_WORD;
      else{
        longword prod = (longword)rp[i] * (longword)di + 16384;
        prod >>= 15;
        temp = prod & 0xFFFF;
      }

      sav  = ui + temp;

      if(rp[i] == MIN_WORD && ui == MIN_WORD) temp = MAX_WORD;
      else{
        longword prod = (longword)rp[i] * (longword)ui + 16384;
        prod >>= 15;
        temp = prod & 0xFFFF;
      }

      di   = di + temp;
    }
    s[k] = di;
  }
}

void Search_for_the_maximum_cross_correlation(word * d, word scal, register word * dp, word * Nc, longword * L_max)
{
       	word i,j;
	longword acc,temp;

        *Nc=40;
	*L_max=0;
	for (i = 40; i <= 120; i ++) {
		acc = 0;
		for (j = 0; j <= 39; j++) {
			temp = ((d[j] >> scal) * dp[j-i]) << 1;
			acc = acc + temp;
		}
		if(acc > *L_max){
			*Nc = i;
			*L_max = acc;
		}
	}
}

word Search_of_the_optimum_scaling(register word * d)
{
	word dmax = 0,scal;
	register word temp;
	register int k;

	for (k = 0; k <= 39; k++) {
		temp = d[k];
		temp = gsm_abs( temp );
		if (temp > dmax) dmax = temp;
	}
	temp = 0;
	if (dmax == 0) scal = 0;
	else {
		temp = gsm_norm( (longword)dmax << 16 );
	}
	if (temp > 6) scal = 0;
	else scal = 6 - temp;      /*sub*/
return scal;
}


void Rescaling_of_L_max(longword * L_max,word scal)
{
  *L_max = *L_max >> (gsm_sub(6, scal));	/* sub(6, scal) */
}


void Compute_the_power_of_the_reconstructed_short_term(register word * dp,word * Nc_out,longword * L_power)
{
	register longword L_temp;
	int k;
	*L_power = 0;
	for (k = 0; k <= 39; k++) {
		L_temp   = SASR( dp[k - *Nc_out], 3 );
		L_temp	 = gsm_L_mult ( L_temp, L_temp);
		*L_power  = gsm_L_add (*L_power, L_temp);
	}
}

void Calculation_of_the_LTP_parameters (
	register word	* d,		/* [0..39]	IN	*/
	register word	* dp,		/* [-120..-1]	IN	*/
	word		* bc_out,	/* 		OUT	*/
	word		* Nc_out	/* 		OUT	*/
)
{
	word		   bc;
	longword	L_max, L_power;
	word		R, S, scal;
	register word	temp;
	word gsm_DLB[4] = {  6554,    16384,	26214,	   32767	};


	scal=Search_of_the_optimum_scaling(d);

	Search_for_the_maximum_cross_correlation(d,scal,dp,Nc_out,&L_max);

	Rescaling_of_L_max(&L_max,scal);

	Compute_the_power_of_the_reconstructed_short_term(dp,Nc_out,&L_power);


	if (L_max <= 0)  {
		*bc_out = 0;
		return;
	}
	if (L_max >= L_power) {
		*bc_out = 3;
		return;
	}

	temp = gsm_norm( L_power );

	R = SASR( L_max   << temp, 16 );
	S = SASR( L_power << temp, 16 );

	for (bc = 0; bc <= 2; bc++) if (R <= gsm_mult(S, gsm_DLB[bc])) break;
	*bc_out = bc;
}


void Long_term_analysis_filtering (
	word		bc,	/* 					IN  */
	word		Nc,	/* 					IN  */
	register word	* dp,	/* previous d	[-120..-1]		IN  */
	register word	* d,	/* d		[0..39]			IN  */
	register word	* dpp,	/* estimate	[0..39]			OUT */
	register word	* e	/* long term res. signal [0..39]	OUT */
)
{
	register int      k;

	switch (bc) {
	case 0:
        	for (k = 0; k <= 39; k++) {
        		dpp[k]  = gsm_mult_r( 3277, dp[k - Nc]);
      		  e[k]	= gsm_sub( d[k], dpp[k] );
        	}
        break;
  case 1:
        	for (k = 0; k <= 39; k++) {
        		dpp[k]  = gsm_mult_r( 11469, dp[k - Nc]);
      		  e[k]	= gsm_sub( d[k], dpp[k] );
        	}
        break;
  case 2:
        	for (k = 0; k <= 39; k++) {
        		dpp[k]  = gsm_mult_r( 21299, dp[k - Nc]);
      		  e[k]	= gsm_sub( d[k], dpp[k] );
        	}
        break;
  case 3:
        	for (k = 0; k <= 39; k++) {
        		dpp[k]  = gsm_mult_r( 32767, dp[k - Nc]);
      		  e[k]	= gsm_sub( d[k], dpp[k] );
        	}
        break;
	}
}




void Weighting_filter (register word * e, word * x)
{
	register int k,i ;
        int acc,temp;
	word wt[50];
	word gsm_H[11] = {-134, -374, 0, 2054, 5741, 8192, 5741, 2054, 0, -374, -134 };


	for (k =  0; k <=  4; k++) wt[k] = 0;
	for (k =  5; k <= 44; k++) wt[k] = e[k-5];
	for (k = 45; k <= 49; k++) wt[k] = 0;

	for (k = 0; k <= 39; k++) {
		acc = 8192;
		for (i = 0; i <= 10; i++) {
			temp   = (wt[k+i] * gsm_H[i]) << 1;
			acc = acc + temp;
		}
	x[k] = acc >> 14 ;
	}

}

void RPE_grid_selection (word * x, word * xM, word * Mc_out)
{
	register int		i;
	register longword	L_result, L_temp;
	longword		EM;
	word			Mc;

	longword		L_common_0_3;

	EM = 0;
	Mc = 0;
#undef	STEP
#define	STEP( m, i )		L_temp = SASR( x[m + 3 * i], 2 ); L_result += L_temp * L_temp;

	L_result = 0;
	STEP( 0, 1 ); STEP( 0, 2 ); STEP( 0, 3 ); STEP( 0, 4 );
	STEP( 0, 5 ); STEP( 0, 6 ); STEP( 0, 7 ); STEP( 0, 8 );
	STEP( 0, 9 ); STEP( 0, 10); STEP( 0, 11); STEP( 0, 12);
	L_common_0_3 = L_result;

	STEP( 0, 0 );
	L_result <<= 1;	/* implicit in L_MULT */
	EM = L_result;

	L_result = 0;
	STEP( 1, 0 );
	STEP( 1, 1 ); STEP( 1, 2 ); STEP( 1, 3 ); STEP( 1, 4 );
	STEP( 1, 5 ); STEP( 1, 6 ); STEP( 1, 7 ); STEP( 1, 8 );
	STEP( 1, 9 ); STEP( 1, 10); STEP( 1, 11); STEP( 1, 12);
	L_result <<= 1;
	if (L_result > EM) {
		Mc = 1;
	 	EM = L_result;
	}

	L_result = 0;
	STEP( 2, 0 );
	STEP( 2, 1 ); STEP( 2, 2 ); STEP( 2, 3 ); STEP( 2, 4 );
	STEP( 2, 5 ); STEP( 2, 6 ); STEP( 2, 7 ); STEP( 2, 8 );
	STEP( 2, 9 ); STEP( 2, 10); STEP( 2, 11); STEP( 2, 12);
	L_result <<= 1;
	if (L_result > EM) {
		Mc = 2;
	 	EM = L_result;
	}

	L_result = L_common_0_3;
	STEP( 3, 12 );
	L_result <<= 1;
	if (L_result > EM) {
		Mc = 3;
	 	EM = L_result;
	}


        for (i = 0; i <= 12; i ++) xM[i] = x[Mc + 3*i];
	*Mc_out = Mc;
}

void APCM_quantization_xmaxc_to_exp_mant (word xmaxc, word * exp_out, word * mant_out )
{
	word	exp, mant;
	exp = 0;
	if (xmaxc > 15) exp = SASR(xmaxc, 3) - 1;
	mant = xmaxc - (exp << 3);

        if (mant == 0) {
		exp  = -4;
		mant = 7;
	}
	else {
		while (mant <= 7) {
			mant = mant << 1 | 1;
			exp--;
		}
		mant -= 8;
	}

	*exp_out  = exp;
	*mant_out = mant;
}

void APCM_quantization (word * xM, word * xMc, word * mant_out,word * exp_out,word * xmaxc_out)
{
	int	i, itest;

	word	xmax, xmaxc, temp, temp1, temp2;
	word	exp, mant;
	word gsm_NRFAC[8] = { 29128, 26215, 23832, 21846, 20165, 18725, 17476, 16384 };

 	xmax = 0;
	for (i = 0; i <= 12; i++) {
		temp = xM[i];
		temp = gsm_abs(temp);
		if (temp > xmax) xmax = temp;
	}

	exp   = 0;
	temp  = SASR( xmax, 9 );
	itest = 0;

	for (i = 0; i <= 5; i++) {

		itest |= (temp <= 0);
		temp = SASR( temp, 1 );


		if (itest == 0) exp++;		/* exp = add (exp, 1) */
	}


	temp = exp + 5;


	xmaxc = gsm_add( SASR(xmax, temp), exp << 3 );

	APCM_quantization_xmaxc_to_exp_mant( xmaxc, &exp, &mant );

	temp1 = 6 - exp;		/* normalization by the exponent */
	temp2 = gsm_NRFAC[ mant ];  	/* inverse mantissa 		 */

	for (i = 0; i <= 12; i++) {


		temp = xM[i] << temp1;
		temp = gsm_mult( temp, temp2 );
		temp = SASR(temp, 12);
		xMc[i] = temp + 4;		/* see note below */
	}

	/*  NOTE: This equation is used to make all the xMc[i] positive.
	 */

	*mant_out  = mant;
	*exp_out   = exp;
	*xmaxc_out = xmaxc;
}

void APCM_inverse_quantization (register word	* xMc,word mant,word exp, register word	* xMp)
{
	int	i;
	word	temp, temp1, temp2, temp3;
	word gsm_FAC[8]	= { 18431, 20479, 22527, 24575, 26623, 28671, 30719, 32767 };



	temp1 = gsm_FAC[ mant ];	/* see 5.2-15 for mant */
	temp2 = gsm_sub( 6, exp );	/* see 5.2-15 for exp  */
	temp3 = 1 << gsm_sub( temp2, 1 );


    for (i = 0; i<=12 ; i++) {
	    temp = gsm_sub( xMc[i] << 1, 7 );
		temp <<= 12;				/* 16 bit signed  */
		temp = gsm_mult_r( temp1, temp );
		temp = gsm_add( temp, temp3 );
		xMp[i] = temp >> temp2;
	}
}

void RPE_grid_positioning (word Mc, register word * xMp, register word * ep)
{
	int	i = 13;

	    switch (Mc) {
                case 3: *ep++ = 0;
                case 2:  do {
                                *ep++ = 0;
                         *ep++ = 0;
                         *ep++ = *xMp++;
                         } while (--i);
				break;
                case 1:         *ep++ = 0;
                case 0:         *ep++ = *xMp++;

        }
        while (++Mc < 4) *ep++ = 0;
}

void Dump(word * S_LARpp0,word *S_LARpp1,word *S_dp0){ //in/out, in/out, in,out

    int l;
    word tmp;
        
    for(l=0;l<8;l++) {
      tmp=S_LARpp0[l];
      S_LARpp0[l]=S_LARpp1[l];
      S_LARpp1[l]=tmp;
    }
    
    for(l=0;l<120;l++)
      S_dp0[l]=S_dp0[l+160];
}


void Read_file(char * filein, word *Data){
  FILE * InFile;
  int num=0;
  int cc, fine=0,i;

  InFile = fopen((char *)filein, "rb");
  if (InFile == NULL)
  {
    printf("error: can not open input file \n");
    exit(1);
  }

  do{
    cc=fread(Data+num*160,1,320,InFile);
    printf("reading file cc %d   n %d\n",cc,num);
    if(cc==0) {fine=1; break;}
    if(cc!=320) {
      fine=1;
      printf("\n L'ultimo frame non �completo, sar�completato con degli zeri");
      for(i=(cc/2)+1;i<160;i++)
        Data[num*160+i]=0;
    }
    num++;
  }while(fine!=1);
  fclose(InFile);
}


void Write_file(char * fileout, unsigned char * c){

  FILE * OutFile;
  int i;
  unsigned char Data[33*NUMFRAME];

  for(i=0;i<33*NUMFRAME;i++)
    Data[i]=c[i];

  OutFile= fopen((char *)fileout, "wb");
  if (OutFile == NULL)
  {
    printf("error: can not open output file \n");
    exit(1);
  }

  fwrite((char *)Data, sizeof(Data), 1, OutFile);
  fclose(OutFile);
}



int done(int * j, int num){
	*j=*j+1;
  if(*j < num) return 0;
  return 1;
}

#define	saturate(x) ((x) < MIN_WORD ? MIN_WORD : (x) > MAX_WORD ? MAX_WORD: (x))


word gsm_add (word a, word b)
{
	longword sum = (longword)a + (longword)b;
	return saturate(sum);
}

word gsm_sub (word a, word b)
{
	longword diff = (longword)a - (longword)b;
	return saturate(diff);
}

word gsm_mult (word a, word b)
{
	if (a == MIN_WORD && b == MIN_WORD) return MAX_WORD;
	else return SASR( (longword)a * (longword)b, 15 );
}

word gsm_mult_r (word a, word b)
{
	if (b == MIN_WORD && a == MIN_WORD) return MAX_WORD;
	else {
		longword prod = (longword)a * (longword)b + 16384;
		prod >>= 15;
		return prod & 0xFFFF;
	}
}

word gsm_abs (word a)
{
	return a < 0 ? (a == MIN_WORD ? MAX_WORD : -a) : a;
}

longword gsm_L_mult (word a, word b)
{
	return ((longword)a * (longword)b) << 1;
}

longword gsm_L_add (longword a, longword b)
{
	if (a < 0) {
		if (b >= 0) return a + b;
		else {
			ulongword A = (ulongword)-(a + 1) + (ulongword)-(b + 1);
			return A >= MAX_LONGWORD ? MIN_LONGWORD :-(longword)A-2;
		}
	}
	else if (b <= 0) return a + b;
	else {
		ulongword A = (ulongword)a + (ulongword)b;
		return A > MAX_LONGWORD ? MAX_LONGWORD : A;
	}
}

longword gsm_L_sub (longword a, longword b)
{
	if (a >= 0) {
		if (b >= 0) return a - b;
		else {
			/* a>=0, b<0 */

			ulongword A = (ulongword)a + -(b + 1);
			return A >= MAX_LONGWORD ? MAX_LONGWORD : (A + 1);
		}
	}
	else if (b <= 0) return a - b;
	else {
		/* a<0, b>0 */  

		ulongword A = (ulongword)-(a + 1) + b;
		return A >= MAX_LONGWORD ? MIN_LONGWORD : -(longword)A - 1;
	}
}

static unsigned char bitoff[ 256 ] = {
	 8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,
	 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

word gsm_norm (longword a )
/*
 * the number of left shifts needed to normalize the 32 bit
 * variable L_var1 for positive values on the interval
 *
 * with minimum of
 * minimum of 1073741824  (01000000000000000000000000000000) and 
 * maximum of 2147483647  (01111111111111111111111111111111)
 *
 *
 * and for negative values on the interval with
 * minimum of -2147483648 (-10000000000000000000000000000000) and
 * maximum of -1073741824 ( -1000000000000000000000000000000).
 *
 * in order to normalize the result, the following
 * operation must be done: L_norm_var1 = L_var1 << norm( L_var1 );
 *
 * (That's 'ffs', only from the left, not the right..)
 */
{


	if (a < 0) {
		if (a <= -1073741824) return 0;
		a = ~a;
	}

	return    a & 0xffff0000 
		? ( a & 0xff000000
		  ?  -1 + bitoff[ 0xFF & (a >> 24) ]
		  :   7 + bitoff[ 0xFF & (a >> 16) ] )
		: ( a & 0xff00
		  ?  15 + bitoff[ 0xFF & (a >> 8) ]
		  :  23 + bitoff[ 0xFF & a ] );
}

longword gsm_L_asl (longword a, int n)
{
	if (n >= 32) return 0;
	if (n <= -32) return -(a < 0);
	if (n < 0) return gsm_L_asr(a, -n);
	return a << n;
}

word gsm_asl (word a, int n)
{
	if (n >= 16) return 0;
	if (n <= -16) return -(a < 0);
	if (n < 0) return gsm_asr(a, -n);
	return a << n;
}

longword gsm_L_asr (longword a, int n)
{
	if (n >= 32) return -(a < 0);
	if (n <= -32) return 0;
	if (n < 0) return a << -n;

	return a >> n;
/*if (a >= 0) return a >> n;
else return -(longword)( -(ulongword)a >> n );*/
}

word gsm_asr (word a, int n)
{
	if (n >= 16) return -(a < 0);
	if (n <= -16) return 0;
	if (n < 0) return a << -n;

	return a >> n;
/*if (a >= 0) return a >> n;
else return -(word)( -(uword)a >> n );*/
}

/* 
 *  (From p. 46, end of section 4.2.5)
 *
 *  NOTE: The following lines gives [sic] one correct implementation
 *  	  of the div(num, denum) arithmetic operation.  Compute div
 *        which is the integer division of num by denum: with denum
 *	  >= num > 0
 */

word gsm_div (word num, word denum)
{
	longword	L_num   = num;
	longword	L_denum = denum;
	word		div 	= 0;
	int		k 	= 15;

	/* The parameter num sometimes becomes zero.
	 * Although this is explicitly guarded against in 4.2.5,
	 * we assume that the result should then be zero as well.
	 */

	if (num == 0)
	    return 0;

	while (k--) {
		div   <<= 1;
		L_num <<= 1;

		if (L_num >= L_denum) {
			L_num -= L_denum;
			div++;
		}
	}

	return div;
}

int main(int argc, char *argv[])
{

  int fine=0,j=0,k;
    
  word		S_dp0[ 280 ] = {0};
  word		S_z1;		/* preprocessing.c, Offset_com. */
  longword	S_L_z2;		/*                  Offset_com. */
//  int		S_mp;		/*                  Preemphasis	*/
  word		S_mp;		/*                  Preemphasis	*/
  word		S_u[8] ;		/* short_term_aly_filter.c	*/
  word		S_LARpp0[8];
  word		S_LARpp1[8];
  
  word s[ 160*NUMFRAME ];
  unsigned char c[33*NUMFRAME];
  
  word LARc[8];
  word Nc[4];
  word Mc[4];
  word bc[4];
  word xmaxc[4];
  word xMc[52];
  word e [50] = {0};
  word so[160];
  longword L_ACF[9];
  word LARp[8];
  word x[40];
  word xM[13];
  word xMp[13];
  word mant, exp;

int i;
  Read_file("short10.inp",s);
  for(i=0; i<8; i++)
    S_u[i]=0;
  
  do{
    Gsm_Preprocess(&S_z1, &S_L_z2, &S_mp, s+j*160, so);
    Autocorrelation(so, L_ACF);
    Reflection_coefficients (L_ACF, LARc);
    Transformation_to_Log_Area_Ratios (LARc);
    Quantization_and_coding (LARc);


    Decoding_of_the_coded_Log_Area_Ratios( LARc, S_LARpp0);
    
    Coefficients_0_12(  S_LARpp1, S_LARpp0, LARp );
    LARp_to_rp( LARp );
    Short_term_analysis_filtering( S_u, LARp, 13, so);
    
    Coefficients_13_26( S_LARpp1, S_LARpp0, LARp);
    LARp_to_rp( LARp );
    Short_term_analysis_filtering( S_u, LARp, 14, so + 13);
    
    Coefficients_27_39( S_LARpp1, S_LARpp0, LARp);
    LARp_to_rp( LARp );
    Short_term_analysis_filtering( S_u, LARp, 13, so + 27);
    
    Coefficients_40_159( S_LARpp0, LARp);
    LARp_to_rp( LARp );
    Short_term_analysis_filtering( S_u, LARp, 120, so + 40);



    for (k = 0; k <= 3; k++) {
      for(i=0;i<280;i++)
   	  printf("WATCH 1  S_dp0[%d]=%d \n",i,S_dp0[i]);
				
      Calculation_of_the_LTP_parameters( so+k*40, S_dp0+120+k*40, bc+k, Nc+k );
      for(i=0;i<280;i++)
   	  printf("WATCH 2  S_dp0[%d]=%d \n",i,S_dp0[i]);

      Long_term_analysis_filtering( bc[k],Nc[k],S_dp0+120+k*40,so+k*40,S_dp0+120+k*40,e+5 );


      Weighting_filter(e+5, x);
      RPE_grid_selection(x, xM, Mc+k);
      APCM_quantization( xM, xMc+k*13, &mant, &exp, xmaxc+k);
      APCM_inverse_quantization(  xMc+k*13,  mant,  exp, xMp);
      RPE_grid_positioning( Mc[k], xMp, e+5);

      Increment(S_dp0+120+k*40,S_dp0+120+k*40,e);
    }

    Gsm_gen(LARc, Nc, bc, Mc, xmaxc, xMc,c+j*33);
        
    Dump(S_LARpp0,S_LARpp1,S_dp0);
      
    fine=done(&j,NUMFRAME);
  }while(!fine);


  Write_file("short10.out",c);
  return 0;
}

