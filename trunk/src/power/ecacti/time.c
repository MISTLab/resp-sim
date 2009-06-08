/*------------------------------------------------------------
 * The following is the license agreement for all components in this
 * archive with exceptions listed in Section II.
 *
 * I. LICENSE
 *
 * Copyright (c)2004 Mahesh Mamidipaka and Nikil Dutt. All rights reserved.
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following four paragraphs appear in all
 * copies of this software, whether in binary form or not.

 * IN NO EVENT SHALL THE AUTHORS, THE UNIVERSITY OF CALIFORNIA, OR THE
 * STATE OF CALIFORNIA BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
 * SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE
 * OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THEY HAVE BEEN ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE AUTHORS, THE UNIVERSITY OF CALIFORNIA, AND THE STATE OF CALIFORNIA
 * SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND
 * THE AUTHORS HAVE NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
 * UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * NEITHER THE NAME OF ANY CALIFORNIA ENTITY NOR THE NAMES OF THE
 * CONTRIBUTORS MAY BE USED TO ENDORSE OR PROMOTE PRODUCTS
 * DERIVED FROM THIS SOFTWARE WITHOUT SPECIFIC PRIOR WRITTEN
 * PERMISSION.
 *
 * If you use this software or a modified version of it, please cite the
 * following paper or an appropriate updated version by the same authors:
 * Mahesh Mamidipaka and Nikil Dutt. "eCACTI: An Enhanced Power Estimation
 * Model for On-chip Caches". Center for Embedded Computer Systems,
 * Technical Report TR 04-28, Oct. 2004.
 *
 * II. EXCEPTIONS
 *                              CACTI 3.0
 *               Copyright 2002 Compaq Computer Corporation
 *                         All Rights Reserved
 *
 * Permission to use, copy, and modify this software and its documentation is
 * hereby granted only under the following terms and conditions.  Both the
 * above copyright notice and this permission notice must appear in all copies
 * of the software, derivative works or modified versions, and any portions
 * thereof, and both notices must appear in supporting documentation.
 *
 * Users of this software agree to the terms and conditions set forth herein,
 * and hereby grant back to Compaq a non-exclusive, unrestricted, royalty-
 * free right and license under any changes, enhancements or extensions
 * made to the core functions of the software, including but not limited to
 * those affording compatibility with other hardware or software
 * environments, but excluding applications which incorporate this software.
 * Users further agree to use their best efforts to return to Compaq any
 * such changes, enhancements or extensions that they make and inform Compaq
 * of noteworthy uses of this software.  Correspondence should be provided
 * to Compaq at:
 *
 *                       Director of Licensing
 *                       Western Research Laboratory
 *                       Compaq Computer Corporation
 *                       250 University Avenue
 *                       Palo Alto, California  94301
 *
 * This software may be distributed (but not offered for sale or transferred
 * for compensation) to third parties, provided such third parties agree to
 * abide by the terms and conditions of this notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND COMPAQ COMPUTER CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL COMPAQ COMPUTER
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *------------------------------------------------------------*/

#include <math.h>
#include "def.h"
#include "leakage.h"
#include "areadef.h"
#include "stdio.h"

extern double calculate_area(area_type,double);

int powers (int base, int n) {
int i,p;

  p = 1;
  for (i = 1; i <= n; ++i)
     p *= base;
  return p;
}

/*----------------------------------------------------------------------*/

double logtwo(double x)
{
if (x<=0) printf("%e\n",x);
	return( (double) (log(x)/log(2.0)) );
}
/*----------------------------------------------------------------------*/

double gatecap(double width, double wirelength)
	/* returns gate capacitance in Farads */
	/* width: gate width in um (length is Leff) */
	/* wirelength: poly wire length going to gate in lambda */
{
	return(width*Leff*Cgate+wirelength*Cpolywire*Leff);
}

double gatecappass(double width, double wirelength)
	/* returns gate capacitance in Farads */
	/* width: gate width in um (length is Leff) */
	/* wirelength: poly wire length going to gate in lambda */
{
        return(width*Leff*Cgatepass+wirelength*Cpolywire*Leff);
}


/*----------------------------------------------------------------------*/

/* Routine for calculating drain capacitances.  The draincap routine
 * folds transistors larger than 10um */

double draincap(double width, int nchannel, int stack)
	/* returns drain cap in Farads */
	/* width: um */
	/* nchannel: whether n or p-channel (boolean) */
	/* stack: number of transistors in series that are on */
{
	double Cdiffside,Cdiffarea,Coverlap,cap;

	Cdiffside = (nchannel) ? Cndiffside : Cpdiffside;
	Cdiffarea = (nchannel) ? Cndiffarea : Cpdiffarea;
	Coverlap = (nchannel) ? (Cndiffovlp+Cnoxideovlp) :
				(Cpdiffovlp+Cpoxideovlp);
	/* calculate directly-connected (non-stacked) capacitance */
	/* then add in capacitance due to stacking */
	if (width >= 10) {
	    cap = 3.0*Leff*width/2.0*Cdiffarea + 6.0*Leff*Cdiffside +
		width*Coverlap;
	    cap += (double)(stack-1)*(Leff*width*Cdiffarea +
		4.0*Leff*Cdiffside + 2.0*width*Coverlap);
	} else {
	    cap = 3.0*Leff*width*Cdiffarea + (6.0*Leff+width)*Cdiffside +
		width*Coverlap;
	    cap += (double)(stack-1)*(Leff*width*Cdiffarea +
		2.0*Leff*Cdiffside + 2.0*width*Coverlap);
	}
	return(cap);
}

/*----------------------------------------------------------------------*/

/* The following routines estimate the effective resistance of an
   on transistor as described in the tech report.  The first routine
   gives the "switching" resistance, and the second gives the
   "full-on" resistance */

double transresswitch(double width, int nchannel, int stack)
	/* returns resistance in ohms */
	/* width:	 um */
	/* nchannel: whether n or p-channel (boolean) */
	/* stack: number of transistors in series */
{
	double restrans;
        restrans = (nchannel) ? (Rnchannelstatic):
                                (Rpchannelstatic);
	/* calculate resistance of stack - assume all but switching trans
	   have 0.8X the resistance since they are on throughout switching */
	return((1.0+((stack-1.0)*0.8))*restrans/width);
}

/*----------------------------------------------------------------------*/

double transreson(double width,int nchannel,int stack)
	/* returns resistance in ohms */
	/* width: um */
	/* nchannel: whether n or p-channel (boolean) */
	/* stack: number of transistors in series */
{
        double restrans;
        restrans = (nchannel) ? Rnchannelon : Rpchannelon;

      /* calculate resistance of stack.  Unlike transres, we don't
           multiply the stacked transistors by 0.8 */
        return(stack*restrans/width);

}

/*----------------------------------------------------------------------*/

/* This routine operates in reverse: given a resistance, it finds
 * the transistor width that would have this R.  It is used in the
 * data wordline to estimate the wordline driver size. */

double restowidth(double res,int nchannel)
	/* returns width in um */
	/* res: resistance in ohms */
	/* nchannel: whether N-channel or P-channel */
{
   double restrans;

        restrans = (nchannel) ? Rnchannelon : Rpchannelon;

   return(restrans/res);

}

/*----------------------------------------------------------------------*/

double horowitz(double inputramptime,double tf,double vs1,double vs2,int rise)
	/* inputramptime: input rise time */
    /* tf: time constant of gate */
    /* vs1,vs2: threshold voltages */
	/* rise: whether INPUT rise or fall (boolean) */
{
    double a,b,td;

    a = inputramptime/tf;
    if (rise==RISE) {
       b = 0.5;
       td = tf*sqrt( log(vs1)*log(vs1)+2*a*b*(1.0-vs1)) +
            tf*(log(vs1)-log(vs2));
    } else {
       b = 0.4;
       td = tf*sqrt( log(1.0-vs1)*log(1.0-vs1)+2*a*b*(vs1)) +
            tf*(log(1.0-vs1)-log(1.0-vs2));
    }
    return(td);
}



/*======================================================================*/



/*
 * This part of the code contains routines for each section as
 * described in the tech report.  See the tech report for more details
 * and explanations */

/*----------------------------------------------------------------------*/

void subbank_routing_length(int C,int B,int A,char fullyassoc,int Ndbl,int Nspd,int Ndwl,int Ntbl,int Ntwl,int Ntspd, double NSubbanks,double *subbank_v,double *subbank_h)
{
double htree;
int htree_int, tagbits;
int cols_data_subarray,rows_data_subarray, cols_tag_subarray,rows_tag_subarray;
double inter_v,inter_h ,sub_h, sub_v;
double inter_subbanks;
int cols_fa_subarray,rows_fa_subarray;

	if (!fullyassoc) {

  		cols_data_subarray = (8*B*A*Nspd/Ndwl);
        rows_data_subarray = (C/(B*A*Ndbl*Nspd));

        if(Ndwl*Ndbl==1) {
	        sub_v= rows_data_subarray;
            sub_h= cols_data_subarray;
        }
        if(Ndwl*Ndbl==2) {
            sub_v= rows_data_subarray;
            sub_h= 2*cols_data_subarray;
        }
        if(Ndwl*Ndbl>2) {
            htree=logtwo((double)(Ndwl*Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
	            sub_v = sqrt(Ndwl*Ndbl)*rows_data_subarray;
                sub_h = sqrt(Ndwl*Ndbl)*cols_data_subarray;
            }
            else {
                sub_v = sqrt(Ndwl*Ndbl/2)*rows_data_subarray;
                sub_h = 2*sqrt(Ndwl*Ndbl/2)*cols_data_subarray;
            }
        }

        inter_v=sub_v;
        inter_h=sub_h;

	    rows_tag_subarray = C/(B*A*Ntbl*Ntspd);
       	tagbits = ADDRESS_BITS+2-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));

        cols_tag_subarray = tagbits*A*Ntspd/Ntwl ;

        if(Ntwl*Ntbl==1) {
        	sub_v= rows_tag_subarray;
            sub_h= cols_tag_subarray;
        }

        if(Ntwl*Ntbl==2) {
            sub_v= rows_tag_subarray;
            sub_h= 2*cols_tag_subarray;
        }

        if(Ntwl*Ntbl>2) {
            htree=logtwo((double)(Ndwl*Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
	            sub_v = sqrt(Ndwl*Ndbl)*rows_tag_subarray;
                sub_h = sqrt(Ndwl*Ndbl)*cols_tag_subarray;
            }
            else {
                sub_v = sqrt(Ndwl*Ndbl/2)*rows_tag_subarray;
                sub_h = 2*sqrt(Ndwl*Ndbl/2)*cols_tag_subarray;
            }
        }


        inter_v=MAX(sub_v,inter_v);
        inter_h+=sub_h;

        sub_v=0;
        sub_h=0;

        if(NSubbanks==1.0 || NSubbanks==2.0 ) {
	        sub_h = 0;
            sub_v = 0;
        }

        if(NSubbanks==4.0) {
            sub_h = 0;
            sub_v = inter_v;
        }

        inter_subbanks=NSubbanks;

		while((inter_subbanks > 2.0) && (NSubbanks > 4.0)) {
	        sub_v+=inter_v;
            sub_h+=inter_h;

            inter_v=2*inter_v;
            inter_h=2*inter_h;
            inter_subbanks=inter_subbanks/4.0;

            if(inter_subbanks==4.0) {
            	inter_h = 0;
			}

		}
        *subbank_v=sub_v;
        *subbank_h=sub_h;
  	}
  	else {
        rows_fa_subarray = (C/(B*Ndbl));
        tagbits = ADDRESS_BITS+2-(int)logtwo((double)B);
        cols_fa_subarray = (8*B)+tagbits;

        if(Ndbl==1) {
	        sub_v= rows_fa_subarray;
            sub_h= cols_fa_subarray;
        }

        if(Ndbl==2) {
            sub_v= rows_fa_subarray;
            sub_h= 2*cols_fa_subarray;
        }

        if(Ndbl>2) {
            htree=logtwo((double)(Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
	            sub_v = sqrt(Ndbl)*rows_fa_subarray;
                sub_h = sqrt(Ndbl)*cols_fa_subarray;
            }
            else {
                sub_v = sqrt(Ndbl/2)*rows_fa_subarray;
                sub_h = 2*sqrt(Ndbl/2)*cols_fa_subarray;
            }
        }

		inter_v=sub_v;
        inter_h=sub_h;

        sub_v=0;
        sub_h=0;

        if(NSubbanks==1.0 || NSubbanks==2.0 ) {
	        sub_h = 0;
            sub_v = 0;
        }

        if(NSubbanks==4.0) {
            sub_h = 0;
            sub_v = inter_v;
        }

        inter_subbanks=NSubbanks;

        while((inter_subbanks > 2.0) && (NSubbanks > 4.0))  {
        	sub_v+=inter_v;
            sub_h+=inter_h;

            inter_v=2*inter_v;
            inter_h=2*inter_h;
            inter_subbanks=inter_subbanks/4.0;

            if(inter_subbanks==4.0) {
            	inter_h = 0;
            }

		}
        *subbank_v=sub_v;
        *subbank_h=sub_h;
  	}
}

void subbank_dim(int C,int B,int A, char fullyassoc, int Ndbl, int Ndwl, int Nspd, int Ntbl, int Ntwl, int Ntspd, double NSubbanks,double *subbank_h, double *subbank_v)
{
	double htree;
	int htree_int, tagbits;
	int cols_data_subarray,rows_data_subarray,cols_tag_subarray,rows_tag_subarray;
	double sub_h, sub_v, inter_v, inter_h;
	int cols_fa_subarray,rows_fa_subarray;

	if (!fullyassoc) {

	    /* calculation of subbank dimensions */
        cols_data_subarray = (8*B*A*Nspd/Ndwl);
        rows_data_subarray = (C/(B*A*Ndbl*Nspd));

        if(Ndwl*Ndbl==1) {
	        sub_v= rows_data_subarray;
            sub_h= cols_data_subarray;
        }

        if(Ndwl*Ndbl==2) {
            sub_v= rows_data_subarray;
            sub_h= 2*cols_data_subarray;
        }

        if(Ndwl*Ndbl>2) {
            htree=logtwo((double)(Ndwl*Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
	            sub_v = sqrt(Ndwl*Ndbl)*rows_data_subarray;
                sub_h = sqrt(Ndwl*Ndbl)*cols_data_subarray;
            }
            else {
	            sub_v = sqrt(Ndwl*Ndbl/2)*rows_data_subarray;
                sub_h = 2*sqrt(Ndwl*Ndbl/2)*cols_data_subarray;
            }
        }

        inter_v=sub_v;
        inter_h=sub_h;

        rows_tag_subarray = C/(B*A*Ntbl*Ntspd);

        tagbits = ADDRESS_BITS+2-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));
        cols_tag_subarray = tagbits*A*Ntspd/Ntwl ;

        if(Ntwl*Ntbl==1) {
	        sub_v= rows_tag_subarray;
            sub_h= cols_tag_subarray;
        }

        if(Ntwl*Ntbl==2) {
            sub_v= rows_tag_subarray;
            sub_h= 2*cols_tag_subarray;
        }

        if(Ntwl*Ntbl>2) {
            htree=logtwo((double)(Ndwl*Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
	            sub_v = sqrt(Ndwl*Ndbl)*rows_tag_subarray;
                sub_h = sqrt(Ndwl*Ndbl)*cols_tag_subarray;
            }
            else {
	            sub_v = sqrt(Ndwl*Ndbl/2)*rows_tag_subarray;
                sub_h = 2*sqrt(Ndwl*Ndbl/2)*cols_tag_subarray;
            }
        }

        inter_v=MAX(sub_v,inter_v);
        inter_h+=sub_h;

		*subbank_v=inter_v;
	   	*subbank_h=inter_h;
  	}

  	else {
        rows_fa_subarray = (C/(B*Ndbl));
        tagbits = ADDRESS_BITS+2-(int)logtwo((double)B);
        cols_fa_subarray = (8*B)+tagbits;

        if(Ndbl==1) {
  	    	sub_v= rows_fa_subarray;
        	sub_h= cols_fa_subarray;
        }
        if(Ndbl==2) {
            sub_v= rows_fa_subarray;
            sub_h= 2*cols_fa_subarray;
        }

        if(Ndbl>2) {
            htree=logtwo((double)(Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
        	    sub_v = sqrt(Ndbl)*rows_fa_subarray;
                sub_h = sqrt(Ndbl)*cols_fa_subarray;
            }
            else {
                sub_v = sqrt(Ndbl/2)*rows_fa_subarray;
                sub_h = 2*sqrt(Ndbl/2)*cols_fa_subarray;
            }
        }

        inter_v=sub_v;
        inter_h=sub_h;

        *subbank_v=inter_v;
        *subbank_h=inter_h;
  	}
}


/*void subbanks_routing_power(fullyassoc,A,NSubbanks,subbank_h,subbank_v,power)
double *subbank_h,*subbank_v;
double NSubbanks;
powerDef *power;
char fullyassoc;
{
	double Ceq,Ceq_outdrv;
	int i,blocks,htree_int,subbank_mod;
	double htree, wire_cap, wire_cap_outdrv, start_h,start_v,line_h,line_v;
	double lkgCurrent=0.0;
	double dynPower=0.0;

	if(fullyassoc) {
		A=1;
	}

	if(NSubbanks==1.0 || NSubbanks==2.0) {
		power->dynamic = 0.0;
		power->leakage = 0.0;
	}

	if(NSubbanks==4.0) {
		//* calculation of address routing power
		wire_cap = Cbitmetal*(*subbank_v);

        Ceq = draincap(Waddrdrvp2,PCH,1)+draincap(Waddrdrvn2,NCH,1)
        		+ gatecap( Waddrdrvp1 + Waddrdrvn1, 0.0);

        dynPower+=2*ADDRESS_BITS*Ceq*.5*VddPow*VddPow;

        lkgCurrent += 2*ADDRESS_BITS*0.5*(nmos_ileakage(Waddrdrvn2/Leff,VddPow,Vthn,Tkelvin,Tox)
						+pmos_ileakage(Waddrdrvp2/Leff,VddPow,Vthp,Tkelvin,Tox) );

        Ceq = draincap(Waddrdrvp1,PCH,1)+draincap(Waddrdrvn1,NCH,1) +
              wire_cap;
        dynPower+=2*ADDRESS_BITS*Ceq*.5*VddPow*VddPow;

        lkgCurrent += 2*ADDRESS_BITS*0.5*(nmos_ileakage(Wdecdriven/Leff,VddPow,Vthn,Tkelvin,Tox)
						+pmos_ileakage(Wdecdrivep/Leff,VddPow,Vthp,Tkelvin,Tox) );


		//* calculation of out driver power
        wire_cap_outdrv = Cbitmetal*(*subbank_v);
        Ceq_outdrv= draincap(Wsenseextdrv1p,PCH,1) + draincap(Wsenseextdrv1n,NCH,1) + gatecap(Wsenseextdrv2n+Wsenseextdrv2p,10.0);

        lkgCurrent += 2*BITOUT*A*muxover*0.5*(nmos_ileakage(Wsenseextdrv1n/Leff,VddPow,Vthn,Tkelvin,Tox)
							+pmos_ileakage(Wsenseextdrv1p/Leff,VddPow,Vthp,Tkelvin,Tox) );

        dynPower+=2*Ceq_outdrv*VddPow*VddPow*.5*BITOUT*A*muxover;


        Ceq_outdrv= draincap(Wsenseextdrv2p,PCH,1) + draincap(Wsenseextdrv2n,NCH,1) + wire_cap_outdrv;
        lkgCurrent += 2*BITOUT*A*muxover*0.5*(nmos_ileakage(Wsenseextdrv2n/Leff,VddPow,Vthn,Tkelvin,Tox)
						+pmos_ileakage(Wsenseextdrv2p/Leff,VddPow,Vthp,Tkelvin,Tox) );

        dynPower+=2*Ceq_outdrv*VddPow*VddPow*.5*BITOUT*A*muxover;

	}

	if(NSubbanks==8.0) {

		wire_cap = Cbitmetal*(*subbank_v)+Cwordmetal*(*subbank_h);
        //* buffer stage
        Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
              gatecap(Wdecdrivep+Wdecdriven,0.0);
        dynPower+=6*ADDRESS_BITS*Ceq*.5*VddPow*VddPow;
        lkgCurrent = 6*ADDRESS_BITS*0.5*(nmos_ileakage(Wdecdriven/Leff,VddPow,Vthn,Tkelvin,Tox)
						+pmos_ileakage(Wdecdrivep/Leff,VddPow,Vthp,Tkelvin,Tox) );


        Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
              wire_cap;
        dynPower+=4*ADDRESS_BITS*Ceq*.5*VddPow*VddPow;
        lkgCurrent += 4*ADDRESS_BITS*0.5*(nmos_ileakage(Wdecdriven/Leff,VddPow,Vthn,Tkelvin,Tox)
						+pmos_ileakage(Wdecdrivep/Leff,VddPow,Vthp,Tkelvin,Tox) );

        Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
              (wire_cap-Cbitmetal*(*subbank_v));
        dynPower+=2*ADDRESS_BITS*Ceq*.5*VddPow*VddPow;
        lkgCurrent += 2*ADDRESS_BITS*0.5*(nmos_ileakage(Wdecdriven/Leff,VddPow,Vthn,Tkelvin,Tox)
						+pmos_ileakage(Wdecdrivep/Leff,VddPow,Vthp,Tkelvin,Tox) );

        //* calculation of out driver power
        wire_cap_outdrv = Cbitmetal*(*subbank_v)+Cwordmetal*(*subbank_h);
        Ceq_outdrv= draincap(Wsenseextdrv1p,PCH,1) + draincap(Wsenseextdrv1n,NCH,1) + gatecap(Wsenseextdrv2n+Wsenseextdrv2p,10.0);
        dynPower+=6*Ceq_outdrv*VddPow*VddPow*.5*BITOUT*A*muxover;
        lkgCurrent += 6*BITOUT*A*muxover*0.5*(nmos_ileakage(Wsenseextdrv1n/Leff,VddPow,Vthn,Tkelvin,Tox)
						+pmos_ileakage(Wsenseextdrv1p/Leff,VddPow,Vthp,Tkelvin,Tox) );

        Ceq_outdrv= draincap(Wsenseextdrv2p,PCH,1) + draincap(Wsenseextdrv2n,NCH,1) + wire_cap_outdrv;
        dynPower+=4*Ceq_outdrv*VddPow*VddPow*.5*BITOUT*A*muxover;
        lkgCurrent += 4*BITOUT*A*muxover*0.5*(nmos_ileakage(Wsenseextdrv2n/Leff,VddPow,Vthn,Tkelvin,Tox)
						+pmos_ileakage(Wsenseextdrv2p/Leff,VddPow,Vthp,Tkelvin,Tox) );

	    Ceq_outdrv= draincap(Wsenseextdrv2p,PCH,1) + draincap(Wsenseextdrv2n,NCH,1) + (wire_cap_outdrv- Cbitmetal*(*subbank_v));
        dynPower+=2*Ceq_outdrv*VddPow*VddPow*.5*BITOUT*A*muxover;
        lkgCurrent += 2*BITOUT*A*muxover*0.5*(nmos_ileakage(Wsenseextdrv2n/Leff,VddPow,Vthn,Tkelvin,Tox)
						+pmos_ileakage(Wsenseextdrv2p/Leff,VddPow,Vthp,Tkelvin,Tox) );
	}

	if(NSubbanks > 8.0) {
		blocks=(int) (NSubbanks/8.0);
	 	htree=logtwo((double)(blocks));
	 	htree_int = (int) htree;
	 	if (htree_int % 2 ==0) {
	 		subbank_mod = htree_int;
	 		start_h=(*subbank_h*(powers(2,((int)(logtwo(htree)))+1)-1));
	 		start_v=*subbank_v;
	 	}
	 	else {
	 		subbank_mod = htree_int -1;
        	start_h=(*subbank_h*(powers(2,(htree_int+1)/2)-1));
        	start_v=*subbank_v;
	 	}

		if(subbank_mod==0) {
			subbank_mod=1;
		}

		line_v= start_v;
		line_h= start_h;

		for(i=1;i<=blocks;i++) {
			wire_cap = line_v*Cbitmetal+line_h*Cwordmetal;

    	    Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
    	          gatecap(Wdecdrivep+Wdecdriven,0.0);
    	    dynPower+=6*ADDRESS_BITS*Ceq*.5*VddPow*VddPow;
    	    lkgCurrent = 6*ADDRESS_BITS*0.5*(nmos_ileakage(Wdecdriven/Leff,VddPow,Vthn,Tkelvin,Tox)
							+pmos_ileakage(Wdecdrivep/Leff,VddPow,Vthp,Tkelvin,Tox) );

		    Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) + wire_cap;
    	    dynPower+=4*ADDRESS_BITS*Ceq*.5*VddPow*VddPow;
    	    lkgCurrent += 4*ADDRESS_BITS*0.5*(nmos_ileakage(Wdecdriven/Leff,VddPow,Vthn,Tkelvin,Tox)
							+pmos_ileakage(Wdecdrivep/Leff,VddPow,Vthp,Tkelvin,Tox) );

    	    Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
    	          (wire_cap-Cbitmetal*(*subbank_v));
    	    dynPower+=2*ADDRESS_BITS*Ceq*.5*VddPow*VddPow;
			lkgCurrent += 2*ADDRESS_BITS*0.5*(nmos_ileakage(Wdecdriven/Leff,VddPow,Vthn,Tkelvin,Tox)
							+pmos_ileakage(Wdecdrivep/Leff,VddPow,Vthp,Tkelvin,Tox) );


    	    //* calculation of out driver power
    	    wire_cap_outdrv = line_v*Cbitmetal+line_h*Cwordmetal;

    	    Ceq_outdrv= draincap(Wsenseextdrv1p,PCH,1) + draincap(Wsenseextdrv1n,NCH,1) + gatecap(Wsenseextdrv2n+Wsenseextdrv2p,10.0);
    	    dynPower+=6*Ceq_outdrv*VddPow*VddPow*.5*BITOUT*A*muxover;
			lkgCurrent += 6*BITOUT*A*muxover*0.5*(nmos_ileakage(Wsenseextdrv1n/Leff,VddPow,Vthn,Tkelvin,Tox)
							+pmos_ileakage(Wsenseextdrv1p/Leff,VddPow,Vthp,Tkelvin,Tox) );

    	    Ceq_outdrv= draincap(Wsenseextdrv2p,PCH,1) + draincap(Wsenseextdrv2n,NCH,1) + wire_cap_outdrv;
    	    dynPower+=4*Ceq_outdrv*VddPow*VddPow*.5*BITOUT*A*muxover;
    	    lkgCurrent += 4*BITOUT*A*muxover*0.5*(nmos_ileakage(Wsenseextdrv2n/Leff,VddPow,Vthn,Tkelvin,Tox)
							+pmos_ileakage(Wsenseextdrv2p/Leff,VddPow,Vthp,Tkelvin,Tox) );

    	    Ceq_outdrv= draincap(Wsenseextdrv2p,PCH,1) + draincap(Wsenseextdrv2n,NCH,1) + (wire_cap_outdrv- Cbitmetal*(*subbank_v));
    	    dynPower+=2*Ceq_outdrv*VddPow*VddPow*.5*BITOUT*A*muxover;
    	    lkgCurrent += 2*BITOUT*A*muxover*0.5*(nmos_ileakage(Wsenseextdrv2n/Leff,VddPow,Vthn,Tkelvin,Tox)
							+pmos_ileakage(Wsenseextdrv2p/Leff,VddPow,Vthp,Tkelvin,Tox) );

			if(i % subbank_mod ==0) {
				line_v+=2*(*subbank_v);
			}
		 }
	}

	power->dynamic = dynPower;
	power->leakage = lkgCurrent * VddPow;
} */

double compute_address_routing_data(int C, int B, int A,char fullyassoc, int Ndwl,int Ndbl,
			int Nspd,int Ntwl,int Ntbl, int Ntspd,double NSubbanks,double *outrisetime,powerDef *power)
{
	double Ceq,Req,Rwire,tf,nextinputtime,delay_stage1,delay_stage2;
    double addr_h,addr_v;
    double wire_cap, wire_res;
    double lkgCurrent = 0.0;
    double dynPower = 0.0;
    double Cline, Cload;


    /* Calculate rise time.  Consider two inverters */

    Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
  	 		gatecap(Wdecdrivep+Wdecdriven,0.0);
    tf = Ceq*transreson(Wdecdriven,NCH,1);
    nextinputtime = horowitz(0.0,tf,VTHINV360x240,VTHINV360x240,FALL)/
                                 (VTHINV360x240);

    Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
   	      	gatecap(Wdecdrivep+Wdecdriven,0.0);
    tf = Ceq*transreson(Wdecdriven,NCH,1);
    nextinputtime = horowitz(nextinputtime,tf,VTHINV360x240,VTHINV360x240,
                               RISE)/(1.0-VTHINV360x240);
	addr_h=0; addr_v=0;
	subbank_routing_length(C,B,A,fullyassoc,Ndbl,Nspd,Ndwl,Ntbl,Ntwl,Ntspd,NSubbanks,&addr_v,&addr_h);
	wire_cap = Cbitmetal*addr_v+addr_h*Cwordmetal;
	wire_res = (Rwordmetal*addr_h + Rbitmetal*addr_v)/2.0;

	/* buffer stage */

	Cline = NSubbanks * (gatecap(Wdecdrivep2+Wdecdriven2,0.0) + gatecap(Wtdecdrivep2+Wtdecdriven2,0.0)) + wire_cap;
	Cload = Cline / gatecap(1.0,0.0);
	Waddrdrvn1 = Cload * SizingRatio /3;
	Waddrdrvp1 = Cload * SizingRatio * 2/3;

	Waddrdrvn2 = (Waddrdrvn1 + Waddrdrvp1) * SizingRatio * 1/3 ;
	Waddrdrvp2 = (Waddrdrvn1 + Waddrdrvp1) * SizingRatio * 2/3 ;

    Ceq = draincap(Waddrdrvp2,PCH,1)+draincap(Waddrdrvn2,NCH,1) +
    		gatecap(Waddrdrvn1+Waddrdrvp1,0.0);
    tf = Ceq*transreson(Waddrdrvn2,NCH,1);
	delay_stage1 = horowitz(nextinputtime,tf,VTHINV360x240,VTHINV360x240,FALL);
    nextinputtime = horowitz(nextinputtime,tf,VTHINV360x240,VTHINV360x240,FALL)/(VTHINV360x240);

    dynPower=ADDRESS_BITS*Ceq*.5*VddPow*VddPow;

    if (dualVt == 1)
		lkgCurrent += ADDRESS_BITS*0.5*(nmos_ileakage(Waddrdrvn2/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox)
					+pmos_ileakage(Waddrdrvp2/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
	    lkgCurrent += ADDRESS_BITS*0.5*(nmos_ileakage(Waddrdrvn2/Leff,VddPow,Vthn,Tkelvin,Tox)
					+pmos_ileakage(Waddrdrvp2/Leff,VddPow,Vthp,Tkelvin,Tox) );

    Ceq = draincap(Waddrdrvp1,PCH,1)+draincap(Waddrdrvn1,NCH,1) + wire_cap
    		+ NSubbanks * (gatecap(Wdecdrivep2+Wdecdriven2,0.0) + gatecap(Wtdecdrivep2+Wtdecdriven2,0.0));
    tf = Ceq*(transreson(Waddrdrvn1,NCH,1)+wire_res);
	delay_stage2=horowitz(nextinputtime,tf,VTHINV360x240,VTHINV360x240,RISE);
    nextinputtime = horowitz(nextinputtime,tf,VTHINV360x240,VTHINV360x240,RISE)/(1.0-VTHINV360x240);

    dynPower+=ADDRESS_BITS*Ceq*.5*VddPow*VddPow;

    if (dualVt == 1)
    	lkgCurrent += ADDRESS_BITS*0.5*(nmos_ileakage(Waddrdrvn1/Leff,VddPow,Vthn,Tkelvin,Tox)
						+pmos_ileakage(Waddrdrvp1/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += ADDRESS_BITS*0.5*(nmos_ileakage(Waddrdrvn1/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox)
						+pmos_ileakage(Waddrdrvp1/Leff,VddPow,Vthp,Tkelvin,Tox) );
	*outrisetime = nextinputtime;

	power->readOp.dynamic = dynPower * FREQ;
	power->readOp.leakage = lkgCurrent * VddPow;

	// power for write op same as read op for address routing
	power->writeOp.dynamic = dynPower * FREQ;
	power->writeOp.leakage = lkgCurrent * VddPow;
	return(delay_stage1+delay_stage2);
}



/* Decoder delay:  (see section 6.1 of tech report) */

double compute_decoder_data(int C, int B,int A,int Ndwl,int Ndbl,int Nspd,int Ntwl,int Ntbl,int Ntspd,double NSubbanks,
			double *Tdecdrive,double *Tdecoder1,double *Tdecoder2,double inrisetime,double *outrisetime, int *nor_inputs, powerDef *power)
{
  	int numstack, Nact;
    double Ceq,Req,Rwire,rows,cols,tf,nextinputtime,vth,tstep,m,a,b,c;
  	double l_inv_predecode, l_predec_nor_v, l_predec_nor_h;
	double wire_cap, wire_res, total_edge_length;
	double htree;
	double lkgCurrent = 0.0, dynPower = 0.0;
	int htree_int,exp;
	double effWdecNORn, effWdecNORp, effWdec3to8p, effWdec3to8n;
	double Cline, Cload;

	int addr_bits=logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd)));

	rows = C/(8*B*A*Ndbl*Nspd);
	cols = 8*B*A*Nspd/Ndwl;

	numstack =
	       ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))));
	if (numstack==0) numstack = 1;

	if (numstack>5) numstack = 5;

	if(Ndwl*Ndbl==1 || Ndwl*Ndbl==2 || Ndwl*Ndbl==4) {
	  	l_predec_nor_v = 0;
	    l_predec_nor_h = 0;
	} else {
		if(Ndwl*Ndbl==8) {
	   	   l_predec_nor_v = 0;
	   	   l_predec_nor_h = cols;
		}
	}

	if(Ndwl*Ndbl>8) {
	    htree=logtwo((double)(Ndwl*Ndbl));
	    htree_int = (int) htree;
	    if (htree_int % 2 ==0) {
	  	    exp = (htree_int/2-1);
	        l_predec_nor_v = (powers(2,exp)-1)*rows*8;
		  	l_predec_nor_h = (powers(2,exp)-1)*cols;
	    } else {
	        exp = (htree_int-3)/2;
	        l_predec_nor_v = (powers(2,exp)-1)*rows*8;
		    l_predec_nor_h = (powers(2,(exp+1))-1)*cols;
	   	}
	}

	// size of address drivers before decoders
	/* First stage: driving the decoders */
	total_edge_length = 8*B*A*Ndwl*Nspd ; //total_edge_length = 8*B*A*Ndbl*Nspd ;
	if(Ndwl*Ndbl==1 ) {
		l_inv_predecode = 0; // l_inv_predecode = 1;
	    wire_cap = Cwordmetal*l_inv_predecode*total_edge_length;
	    wire_res = 0.5*Rwordmetal*l_inv_predecode*total_edge_length;
	}

	if(Ndwl*Ndbl==2 ) {
	    l_inv_predecode = 0.5;
	    wire_cap = Cwordmetal*l_inv_predecode*total_edge_length;
	    wire_res = 0.5*Rwordmetal*l_inv_predecode*total_edge_length;
	}

	if(Ndwl*Ndbl>2) {
		htree=logtwo((double)(Ndwl*Ndbl));
		htree_int = (int) htree;
	    if (htree_int % 2 ==0) {
			exp = (htree_int/2-1);
			l_inv_predecode = 0.25/(powers(2,exp));
	        wire_cap = Cwordmetal*l_inv_predecode*total_edge_length;
	        wire_res = 0.5*Rwordmetal*l_inv_predecode*total_edge_length;
	  	}
	   	else {
			exp = (htree_int-3)/2;
			l_inv_predecode = powers(2,exp);
	        wire_cap = Cbitmetal*l_inv_predecode*rows*8;
	        wire_res = 0.5*Rbitmetal*l_inv_predecode*rows*8;
	   	}
	}

    /* Calculate rise time.  Consider two inverters */

    if (NSubbanks > 2) {
		Ceq = draincap(Waddrdrvp1,PCH,1)+draincap(Waddrdrvn1,NCH,1) +
    			gatecap(Wdecdrivep2+Wdecdriven2,0.0);

    	tf = Ceq*transreson(Waddrdrvn1,NCH,1);
    	nextinputtime = horowitz(0.0,tf,VTHINV360x240,VTHINV360x240,FALL)/
                                  (VTHINV360x240);
	} else {
		Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven1,NCH,1) +
    			gatecap(Wdecdrivep2+Wdecdriven2,0.0);

    	tf = Ceq*transreson(Wdecdriven,NCH,1);
    	nextinputtime = horowitz(0.0,tf,VTHINV360x240,VTHINV360x240,FALL)/
                                  (VTHINV360x240);
	}

	if (dualVt == 1)
		lkgCurrent += ADDRESS_BITS*0.5*(nmos_ileakage(Wdecdriven2/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox)
		 					+pmos_ileakage(Wdecdrivep2/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += ADDRESS_BITS*0.5*(nmos_ileakage(Wdecdriven2/Leff,VddPow,Vthn,Tkelvin,Tox)
		 					+pmos_ileakage(Wdecdrivep2/Leff,VddPow,Vthp,Tkelvin,Tox) );


    Ceq = draincap(Wdecdrivep2,PCH,1)+draincap(Wdecdriven2,NCH,1) +
    		  gatecap(Wdecdrivep1+Wdecdriven1,0.0);

    tf = Ceq*transreson(Wdecdriven2,NCH,1);
    nextinputtime = horowitz(nextinputtime,tf,VTHINV360x240,VTHINV360x240,RISE)/
                                  (1.0-VTHINV360x240);

	if (dualVt == 1)
		lkgCurrent += ADDRESS_BITS*0.5*(nmos_ileakage(Wdecdriven1/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox)
		 					+pmos_ileakage(Wdecdrivep1/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else

		lkgCurrent += ADDRESS_BITS*0.5*(nmos_ileakage(Wdecdriven1/Leff,VddPow,Vthn,Tkelvin,Tox)
		 					+pmos_ileakage(Wdecdrivep1/Leff,VddPow,Vthp,Tkelvin,Tox) );

    Ceq = draincap(Wdecdrivep1,PCH,1)+draincap(Wdecdriven1,NCH,1) +
	 		4*gatecap(Wdec3to8n+Wdec3to8p,10.0)*(Ndwl*Ndbl)+wire_cap;

	// there are 8 nand gates in each 3-8 decoder. since these transistors are
	// stacked, we use a stacking factor of 1/5 (0.2). 0.5 signifies that we
	// are taking the average of both nmos and pmos transistors.

	if (dualVt == 1)
		lkgCurrent += 8*0.2*0.5*(nmos_ileakage(Wdec3to8n/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox)
		 					+pmos_ileakage(Wdec3to8p/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += 8*0.2*0.5*(nmos_ileakage(Wdec3to8n/Leff,VddPow,Vthn,Tkelvin,Tox)
	 					+pmos_ileakage(Wdec3to8p/Leff,VddPow,Vthp,Tkelvin,Tox) );

	// For the all the 3-8 decoders:
	lkgCurrent *= ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))));

    Rwire = wire_res;

	tf = (Rwire + transreson(Wdecdrivep1,PCH,1))*Ceq;

    *Tdecdrive = horowitz(inrisetime,tf,VTHINV360x240,VTHNAND60x120,FALL);

    nextinputtime = *Tdecdrive/VTHNAND60x120;
	dynPower+=addr_bits*Ceq*.5*VddPow*VddPow;

    Ceq = 3*draincap(Wdec3to8p,PCH,1) +draincap(Wdec3to8n,NCH,3) +
	 			gatecap(WdecNORn+WdecNORp,((numstack*40)+20.0))*rows/8 +
	  			Cbitmetal*(rows+l_predec_nor_v)+Cwordmetal*(l_predec_nor_h);
    Rwire = Rbitmetal*(rows+l_predec_nor_v)/2 + Rwordmetal*(l_predec_nor_h)/2;

	tf = Ceq*(Rwire+transreson(Wdec3to8n,NCH,3));

	// 0.2 is the stacking factor, 0.5 for averging of nmos and pmos leakage
	// and since there are rows number of nor gates:

	if (dualVt == 1)
		lkgCurrent += 0.5*0.2* rows *
					(nmos_ileakage(WdecNORn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox)
	 				+pmos_ileakage(WdecNORp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += 0.5*0.2* rows *
					(nmos_ileakage(WdecNORn/Leff,VddPow,Vthn,Tkelvin,Tox)
	 				+pmos_ileakage(WdecNORp/Leff,VddPow,Vthp,Tkelvin,Tox) );

	// number of active blocks among Ndwl modules
	if (Ndwl/Nspd < 1)
		Nact = 1;
	else
		Nact = Ndwl/Nspd;

	//dynPower+=Ndwl*Ndbl*Ceq*VddPow*VddPow*4*ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))));
	dynPower+=0.5*Nact*Ceq*VddPow*VddPow*4*ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))));

    /* we only want to charge the output to the threshold of the
       nor gate.  But the threshold depends on the number of inputs
       to the nor.  */

    switch(numstack) {
       	case 1: vth = VTHNOR12x4x1; break;
       	case 2: vth = VTHNOR12x4x2; break;
       	case 3: vth = VTHNOR12x4x3; break;
       	case 4: vth = VTHNOR12x4x4; break;
       	case 5: vth = VTHNOR12x4x4; break;
	    default: printf("error:numstack=%d\n",numstack);
		printf("Cacti does not support a series stack of %d transistors !\n",numstack);
		exit(0);
		break;
	}

    *Tdecoder1 = horowitz(nextinputtime,tf,VTHNAND60x120,vth,RISE);

    nextinputtime = *Tdecoder1/(1.0-vth);

    /* Final stage: driving an inverter with the nor */

    Req = transreson(WdecNORp,PCH,numstack);

    Ceq = (gatecap(Wdecinvn+Wdecinvp,20.0)+
          numstack * draincap(WdecNORn,NCH,1)+draincap(WdecNORp,PCH,numstack));

	if (dualVt == 1)
	lkgCurrent += 0.5* rows *
					(nmos_ileakage(Wdecinvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox)
		 			+pmos_ileakage(Wdecinvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
	lkgCurrent += 0.5* rows *
					(nmos_ileakage(Wdecinvn/Leff,VddPow,Vthn,Tkelvin,Tox)
		 			+pmos_ileakage(Wdecinvp/Leff,VddPow,Vthp,Tkelvin,Tox) );
    tf = Req*Ceq;

    *Tdecoder2 = horowitz(nextinputtime,tf,vth,VSINV,FALL);

    *outrisetime = *Tdecoder2/(VSINV);
	*nor_inputs=numstack;
	dynPower+=Ceq*VddPow*VddPow;

	//printf("%g %g %g %d %d %d\n",*Tdecdrive,*Tdecoder1,*Tdecoder2,Ndwl, Ndbl,Nspd);

	//fprintf(stderr, "%f %f %f %f %d %d %d\n", (*Tdecdrive+*Tdecoder1+*Tdecoder2)*1e3, *Tdecdrive*1e3, *Tdecoder1*1e3, *Tdecoder2*1e3, Ndwl, Ndbl, Nspd);
	power->readOp.dynamic = dynPower*FREQ;
	power->readOp.leakage = (lkgCurrent * VddPow) * Ndwl * Ndbl;

	power->writeOp.dynamic = dynPower*FREQ;
	power->writeOp.leakage = (lkgCurrent * VddPow) * Ndwl * Ndbl;
    return(*Tdecdrive+*Tdecoder1+*Tdecoder2);
}



/*----------------------------------------------------------------------*/

/* Decoder delay in the tag array (see section 6.1 of tech report) */


double compute_decoder_tag_data(int C, int B,int A,int Ndwl,int Ndbl,int Nspd,int Ntwl,int Ntbl, int Ntspd,double NSubbanks,
             double *Tdecdrive, double *Tdecoder1, double *Tdecoder2,double inrisetime,double *outrisetime, int *nor_inputs, powerDef *power)
{
    double Ceq,Req,Rwire,rows,cols,tf,nextinputtime,vth,tstep,m,a,b,c;
	int numstack,tagbits, Nact;
	double htree;
	int htree_int,exp;
	double l_inv_predecode,l_predec_nor_v,l_predec_nor_h;
	double wire_cap, wire_res, total_edge_length;
	double lkgCurrent=0.0, dynPower = 0.0;
	double Cline, Cload;
	double effWtdec3to8p, effWtdec3to8n, effWtdecNORn, effWtdecNORp;
	int addr_bits=logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd)));

    rows = C/(8*B*A*Ntbl*Ntspd);

	tagbits = ADDRESS_BITS+2-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));

	cols = tagbits*A*Ntspd/Ntwl ;

	numstack =
	    ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd))));
	if (numstack==0) numstack = 1;
	if (numstack>5) numstack = 5;

	if(Ntwl*Ntbl==1 || Ntwl*Ntbl==2 || Ntwl*Ntbl==4) {
		l_predec_nor_v = 0;
	   	l_predec_nor_h = 0;
	} else {
	    if(Ntwl*Ntbl==8) {
			l_predec_nor_v = 0;
			l_predec_nor_h = cols;
        }
    }

	if(Ntwl*Ntbl>8) {
		htree=logtwo((double)(Ntwl*Ntbl));
	    htree_int = (int) htree;
	    if (htree_int % 2 ==0) {
			exp = (htree_int/2-1);
	        l_predec_nor_v = (powers(2,exp)-1)*rows*8;
	        l_predec_nor_h = (powers(2,exp)-1)*cols;
	    }
	    else {
			exp = (htree_int-3)/2;
	        l_predec_nor_v = (powers(2,exp)-1)*rows*8;
	        l_predec_nor_h = (powers(2,(exp+1))-1)*cols;
	    }
	}

	total_edge_length = cols*Ntwl*Ntspd ;

    if(Ntwl*Ntbl==1 ) {
    	l_inv_predecode = 0; //l_inv_predecode = 1;
        wire_cap = Cwordmetal*l_inv_predecode*total_edge_length;
        wire_res = 0.5*Rwordmetal*l_inv_predecode*total_edge_length;
    }

    if(Ntwl*Ntbl==2 ) {
        l_inv_predecode = 0.5;
        wire_cap = Cwordmetal*l_inv_predecode*total_edge_length;
        wire_res = 0.5*Rwordmetal*l_inv_predecode*total_edge_length;
    }

    if(Ntwl*Ntbl>2) {
        htree=logtwo((double)(Ntwl*Ntbl));
        htree_int = (int) htree;
        if (htree_int % 2 ==0) {
       		exp = (htree_int/2-1);
    	    l_inv_predecode = 0.25/(powers(2,exp));
            wire_cap = Cwordmetal*l_inv_predecode*total_edge_length;
            wire_res = 0.5*Rwordmetal*l_inv_predecode*total_edge_length;
        } else {
     		exp = (htree_int-3)/2;
           	l_inv_predecode = powers(2,exp);
           	wire_cap = Cbitmetal*l_inv_predecode*rows*8;
           	wire_res = 0.5*Rbitmetal*l_inv_predecode*rows*8;
        }
	}

    /* Calculate rise time.  Consider two inverters */
    if (NSubbanks > 2) {
		Ceq = draincap(Waddrdrvp1,PCH,1)+draincap(Waddrdrvn1,NCH,1) +
		    		gatecap(Wtdecdrivep2+Wtdecdriven2,0.0);

    	tf = Ceq*transreson(Waddrdrvn1,NCH,1);

    	nextinputtime = horowitz(0.0,tf,VTHINV360x240,VTHINV360x240,FALL)/
                                  (VTHINV360x240);
	} else {
		Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
		    		gatecap(Wtdecdrivep2+Wtdecdriven2,0.0);

    	tf = Ceq*transreson(Wdecdriven,NCH,1);

    	nextinputtime = horowitz(0.0,tf,VTHINV360x240,VTHINV360x240,FALL)/
                                  (VTHINV360x240);
	}

	if (dualVt == 1)
		lkgCurrent = 0.5*(nmos_ileakage(Wtdecdriven2/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox)
					+pmos_ileakage(Wtdecdrivep2/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent = 0.5*(nmos_ileakage(Wtdecdriven2/Leff,VddPow,Vthn,Tkelvin,Tox)
					+pmos_ileakage(Wtdecdrivep2/Leff,VddPow,Vthp,Tkelvin,Tox) );

    nextinputtime = horowitz(nextinputtime,tf,VTHINV360x240,VTHINV360x240,
                        RISE)/(1.0-VTHINV360x240);

    Ceq = draincap(Wtdecdrivep2,PCH,1)+draincap(Wtdecdriven2,NCH,1) +
             gatecap(Wtdecdrivep1+Wtdecdriven1,0.0);
    tf = Ceq*transreson(Wtdecdriven2,NCH,1);

	if (dualVt == 1)
		lkgCurrent += 0.5*(nmos_ileakage(Wtdecdriven1/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox)
				+pmos_ileakage(Wtdecdrivep1/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += 0.5*(nmos_ileakage(Wtdecdriven1/Leff,VddPow,Vthn,Tkelvin,Tox)
				+pmos_ileakage(Wtdecdrivep1/Leff,VddPow,Vthp,Tkelvin,Tox) );

	// for all address bits:
	lkgCurrent *= addr_bits;

    /* First stage: driving the decoders */

    Ceq = draincap(Wtdecdrivep1,PCH,1)+draincap(Wtdecdriven1,NCH,1) +
            4*gatecap(Wtdec3to8n+Wtdec3to8p,10.0)*(Ntwl*Ntbl)+ wire_cap;
    Rwire = wire_res;

    tf = (Rwire + transreson(Wtdecdrivep1,PCH,1))*Ceq;
    *Tdecdrive = horowitz(inrisetime,tf,VTHINV360x240,VTHNAND60x120,FALL);
    nextinputtime = *Tdecdrive/VTHNAND60x120;

    // there are 8 nand gates in each 3-8 decoder. since these transistors are
	// stacked, we use a stacking factor of 1/5 (0.2). 0.5 signifies that we
	// are taking the average of both nmos and pmos transistors.

	if (dualVt == 1)
		lkgCurrent += 8*0.2*0.5*(nmos_ileakage(Wtdec3to8n/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox)
					+pmos_ileakage(Wtdec3to8p/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += 8*0.2*0.5*(nmos_ileakage(Wtdec3to8n/Leff,VddPow,Vthn,Tkelvin,Tox)
					+pmos_ileakage(Wtdec3to8p/Leff,VddPow,Vthp,Tkelvin,Tox) );
	// For the all the 3-8 decoders:
	lkgCurrent *= ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))));

	dynPower+=addr_bits*Ceq*.5*VddPow*VddPow;

    /* second stage: driving a bunch of nor gates with a nand */


    Ceq = 3*draincap(Wtdec3to8p,PCH,1) +draincap(Wtdec3to8n,NCH,3) +
           gatecap(WtdecNORn+WtdecNORp,((numstack*40)+20.0))*rows +
           Cbitmetal*(rows*8 + l_predec_nor_v) +Cwordmetal*(l_predec_nor_h);

    Rwire = Rbitmetal*(rows*8+l_predec_nor_v)/2 + Rwordmetal*(l_predec_nor_h)/2;

    tf = Ceq*(Rwire+transreson(Wtdec3to8n,NCH,3));

    // 0.2 is the stacking factor, 0.5 for averging of nmos and pmos leakage
	// and since there are rows number of nor gates:

	if (dualVt == 1)
		lkgCurrent += 0.5*0.2* rows *
				(nmos_ileakage(WtdecNORn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox)
				+pmos_ileakage(WtdecNORp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += 0.5*0.2* rows *
				(nmos_ileakage(WtdecNORn/Leff,VddPow,Vthn,Tkelvin,Tox)
	 			+pmos_ileakage(WtdecNORp/Leff,VddPow,Vthp,Tkelvin,Tox) );

	// Number of active blocks in Ntwl modules
	if (Ntwl/Ntspd < 1)
		Nact = 1;
	else
		Nact = Ntwl/Ntspd;

	dynPower+=0.5*Nact*Ceq*VddPow*VddPow*4*ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd))));
	//dynPower+=Ntwl*Ntbl*Ceq*VddPow*VddPow*4*ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd))));

    /* we only want to charge the output to the threshold of the
       nor gate.  But the threshold depends on the number of inputs
       to the nor.  */

    switch(numstack) {
	    case 1: vth = VTHNOR12x4x1; break;
        case 2: vth = VTHNOR12x4x2; break;
        case 3: vth = VTHNOR12x4x3; break;
        case 4: vth = VTHNOR12x4x4; break;
        case 5: vth = VTHNOR12x4x4; break;
        case 6: vth = VTHNOR12x4x4; break;
        default: printf("error:numstack=%d\n",numstack);
                   printf("Cacti does not support a series stack of %d transistors !\n",numstack);
		exit(0);
        break;
	}

    *Tdecoder1 = horowitz(nextinputtime,tf,VTHNAND60x120,vth,RISE);
    nextinputtime = *Tdecoder1/(1.0-vth);

    /* Final stage: driving an inverter with the nor */

    Req = transreson(WtdecNORp,PCH,numstack);
    Ceq = (gatecap(Wtdecinvn+Wtdecinvp,20.0)+numstack*draincap(WtdecNORn,NCH,1)+
               draincap(WtdecNORp,PCH,numstack));

    tf = Req*Ceq;
    *Tdecoder2 = horowitz(nextinputtime,tf,vth,VSINV,FALL);
    *outrisetime = *Tdecoder2/(VSINV);
	*nor_inputs=numstack;

	dynPower+=Ceq*VddPow*VddPow;

	if (dualVt == 1)
		lkgCurrent += 0.5* rows *
					(nmos_ileakage(Wtdecinvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox)
		 			+pmos_ileakage(Wtdecinvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += 0.5* rows *
					(nmos_ileakage(Wtdecinvn/Leff,VddPow,Vthn,Tkelvin,Tox)
		 			+pmos_ileakage(Wtdecinvp/Leff,VddPow,Vthp,Tkelvin,Tox) );

	power->readOp.dynamic = dynPower*FREQ;
	power->readOp.leakage = (lkgCurrent * VddPow) * Ntwl * Ntbl;

	power->writeOp.dynamic = dynPower*FREQ;
	power->writeOp.leakage = (lkgCurrent * VddPow) * Ntwl * Ntbl;

    return(*Tdecdrive+*Tdecoder1+*Tdecoder2);
}

double fa_tag_delay(int C,int B,int Ndwl,int Ndbl,int Nspd,int Ntwl,int Ntbl,int Ntspd,
             double *Tagdrive, double *Tag1, double *Tag2,double *Tag3,double *Tag4,double *Tag5,double *outrisetime, int *nor_inputs, powerDef *power)
{
	double Ceq,Req,Rwire,rows,tf,nextinputtime,vth,tstep,m,a,b,c;
	int numstack;
	double Tagdrive1=0, Tagdrive2=0;
	double Tagdrive0a=0, Tagdrive0b=0;
	double TagdriveA=0, TagdriveB=0;
	double TagdriveA1=0, TagdriveB1=0;
	double TagdriveA2=0, TagdriveB2=0;

	double lkgCurrent = 0.0, dynPower=0.0;

    rows = C/(B*Ntbl);


    /* Calculate rise time.  Consider two inverters */

    Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
	  		gatecap(Wdecdrivep+Wdecdriven,0.0);
    tf = Ceq*transreson(Wdecdriven,NCH,1);

  	nextinputtime = horowitz(0.0,tf,VTHINV360x240,VTHINV360x240,FALL)/
									  (VTHINV360x240);

	Ceq = draincap(Wdecdrivep,PCH,1)+draincap(Wdecdriven,NCH,1) +
		  gatecap(Wdecdrivep+Wdecdriven,0.0);
    tf = Ceq*transreson(Wdecdrivep,PCH,1);
    nextinputtime = horowitz(nextinputtime,tf,VTHINV360x240,VTHINV360x240,
						 RISE)/(1.0-VTHINV360x240);

	// If tag bitline divisions, add extra driver

	if (Ntbl>1) {
		Ceq = draincap(Wfadrivep,PCH,1)+draincap(Wfadriven,NCH,1) +
	    	  gatecap(Wfadrive2p+Wfadrive2n,0.0);
	    tf = Ceq*transreson(Wfadriven,NCH,1);
	    TagdriveA = horowitz(nextinputtime,tf,VSINV,VSINV,FALL);
	    nextinputtime = TagdriveA/(VSINV);
	    dynPower+=.5*Ceq*VddPow*VddPow*ADDRESS_BITS;

	    if (Ntbl<=4) {
			Ceq = draincap(Wfadrive2p,PCH,1)+draincap(Wfadrive2n,NCH,1) +
				  gatecap(Wfadecdrive1p+Wfadecdrive1n,10.0)*2+
					  + FACwordmetal*sqrt((rows+1)*Ntbl)/2
						  + FACbitmetal*sqrt((rows+1)*Ntbl)/2;
			Rwire = FARwordmetal*sqrt((rows+1)*Ntbl)*.5/2+
					FARbitmetal*sqrt((rows+1)*Ntbl)*.5/2;
			tf = Ceq*(transreson(Wfadrive2p,PCH,1)+Rwire);
			TagdriveB = horowitz(nextinputtime,tf,VSINV,VSINV,RISE);
			nextinputtime = TagdriveB/(1.0-VSINV);
			dynPower+=Ceq*VddPow*VddPow*ADDRESS_BITS*.5*2;
	    }
	    else {
			Ceq = draincap(Wfadrive2p,PCH,1)+draincap(Wfadrive2n,NCH,1) +
					gatecap(Wfadrivep+Wfadriven,10.0)*2+
				  + FACwordmetal*sqrt((rows+1)*Ntbl)/2
				  + FACbitmetal*sqrt((rows+1)*Ntbl)/2;
			Rwire = FARwordmetal*sqrt((rows+1)*Ntbl)*.5/2+
					 FARbitmetal*sqrt((rows+1)*Ntbl)*.5/2;
			tf = Ceq*(transreson(Wfadrive2p,PCH,1)+Rwire);
			TagdriveB = horowitz(nextinputtime,tf,VSINV,VSINV,RISE);
			nextinputtime = TagdriveB/(1.0-VSINV);
			dynPower+=Ceq*VddPow*VddPow*ADDRESS_BITS*.5*4;

			Ceq = draincap(Wfadrivep,PCH,1)+draincap(Wfadriven,NCH,1) +
				  gatecap(Wfadrive2p+Wfadrive2n,10.0);
			tf = Ceq*transreson(Wfadriven,NCH,1);
			TagdriveA1 = horowitz(nextinputtime,tf,VSINV,VSINV,FALL);
			nextinputtime = TagdriveA1/(VSINV);
			dynPower+=.5*Ceq*VddPow*VddPow*ADDRESS_BITS;

			if (Ntbl<=16)  {
		    	Ceq = draincap(Wfadrive2p,PCH,1)+draincap(Wfadrive2n,NCH,1) +
		    		  gatecap(Wfadecdrive1p+Wfadecdrive1n,10.0)*2+
		    		  + FACwordmetal*sqrt((rows+1)*Ntbl)/4
		    		  + FACbitmetal*sqrt((rows+1)*Ntbl)/4;
		   		Rwire = FARwordmetal*sqrt((rows+1)*Ntbl)*.5/4+
					      FARbitmetal*sqrt((rows+1)*Ntbl)*.5/4;
			    tf = Ceq*(transreson(Wfadrive2p,PCH,1)+Rwire);
			    TagdriveB1 = horowitz(nextinputtime,tf,VSINV,VSINV,RISE);
			    nextinputtime = TagdriveB1/(1.0-VSINV);
			    dynPower+=Ceq*VddPow*VddPow*ADDRESS_BITS*.5*8;
			}
			else
		  	{
		  		Ceq = draincap(Wfadrive2p,PCH,1)+draincap(Wfadrive2n,NCH,1) +
		    		  gatecap(Wfadrivep+Wfadriven,10.0)*2+
		    		  + FACwordmetal*sqrt((rows+1)*Ntbl)/4
		    		  + FACbitmetal*sqrt((rows+1)*Ntbl)/4;
		    	Rwire = FARwordmetal*sqrt((rows+1)*Ntbl)*.5/4+
		    			  FARbitmetal*sqrt((rows+1)*Ntbl)*.5/4;
		    	tf = Ceq*(transreson(Wfadrive2p,PCH,1)+Rwire);
		    	TagdriveB1 = horowitz(nextinputtime,tf,VSINV,VSINV,RISE);
		    	nextinputtime = TagdriveB1/(1.0-VSINV);
		    	dynPower+=Ceq*VddPow*VddPow*ADDRESS_BITS*.5*8;

		    	Ceq = draincap(Wfadrivep,PCH,1)+draincap(Wfadriven,NCH,1) +
		    		  gatecap(Wfadrive2p+Wfadrive2n,10.0);
		    	tf = Ceq*transreson(Wfadriven,NCH,1);
		    	TagdriveA2 = horowitz(nextinputtime,tf,VSINV,VSINV,FALL);
		    	nextinputtime = TagdriveA2/(VSINV);
		    	dynPower+=.5*Ceq*VddPow*VddPow*ADDRESS_BITS;

		    	Ceq = draincap(Wfadrive2p,PCH,1)+draincap(Wfadrive2n,NCH,1) +
		    		  gatecap(Wfadecdrive1p+Wfadecdrive1n,10.0)*2+
		    		  + FACwordmetal*sqrt((rows+1)*Ntbl)/8
		    		  + FACbitmetal*sqrt((rows+1)*Ntbl)/8;
		    	Rwire = FARwordmetal*sqrt((rows+1)*Ntbl)*.5/8+
		    			  FARbitmetal*sqrt((rows+1)*Ntbl)*.5/8;
		    	tf = Ceq*(transreson(Wfadrive2p,PCH,1)+Rwire);
		    	TagdriveB2 = horowitz(nextinputtime,tf,VSINV,VSINV,RISE);
		    	nextinputtime = TagdriveB2/(1.0-VSINV);
		    	dynPower+=Ceq*VddPow*VddPow*ADDRESS_BITS*.5*16;
		  	}
		}
	}

	/* Two more inverters for enable delay */

    Ceq = draincap(Wfadecdrive1p,PCH,1)+draincap(Wfadecdrive1n,NCH,1)
	  		+gatecap(Wfadecdrive2p+Wfadecdrive2n,0.0);
    tf = Ceq*transreson(Wfadecdrive1n,NCH,1);
    Tagdrive0a = horowitz(nextinputtime,tf,VSINV,VSINV, FALL);
	nextinputtime = Tagdrive0a/(VSINV);
	dynPower+=.5*Ceq*VddPow*VddPow*ADDRESS_BITS*Ntbl;

    Ceq = draincap(Wfadecdrive2p,PCH,1)+draincap(Wfadecdrive2n,NCH,1) +
		  +gatecap(Wfadecdrivep+Wfadecdriven,10.0)
		  +gatecap(Wfadecdrive2p+Wfadecdrive2n,10.0);
    tf = Ceq*transreson(Wfadecdrive2p,PCH,1);
    Tagdrive0b = horowitz(nextinputtime,tf,VSINV,VSINV,RISE);
	nextinputtime = Tagdrive0b/(VSINV);
	dynPower+=Ceq*VddPow*VddPow*ADDRESS_BITS*.5*Ntbl;

	/* First stage */

    Ceq = draincap(Wfadecdrive2p,PCH,1)+draincap(Wfadecdrive2n,NCH,1) +
              gatecap(Wfadecdrivep+Wfadecdriven,10.0);
    tf = Ceq*transresswitch(Wfadecdrive2n,NCH,1);
    Tagdrive1 = horowitz(nextinputtime,tf,VSINV,VTHFA1,FALL);
    nextinputtime = Tagdrive1/VTHFA1;
	dynPower+=Ceq*VddPow*VddPow*ADDRESS_BITS*.5*Ntbl;

	Ceq = draincap(Wfadecdrivep,PCH,2)+draincap(Wfadecdriven,NCH,2)
	  		+ draincap(Wfaprechn,NCH,1)
	  		+ gatecap(Wdummyn,10.0)*(rows+1)
	  		+ FACbitmetal*(rows+1);

    Rwire = FARbitmetal*(rows+1)*.5;
    tf = (Rwire + transreson(Wfadecdrivep,PCH,1) +
	      transresswitch(Wfadecdrivep,PCH,1))*Ceq;
    Tagdrive2 = horowitz(nextinputtime,tf,VTHFA1,VTHFA2,RISE);
    nextinputtime = Tagdrive2/(1-VTHFA2);

	*Tagdrive=Tagdrive1+Tagdrive2+TagdriveA+TagdriveB+TagdriveA1+TagdriveA2+TagdriveB1+TagdriveB2+Tagdrive0a+Tagdrive0b;
	dynPower+=Ceq*VddPow*VddPow*ADDRESS_BITS*Ntbl;

    /* second stage */

    Ceq = .5*ADDRESS_BITS*2*draincap(Wdummyn,NCH,2)
			  + draincap(Wfaprechp,PCH,1)
 			  + gatecap(Waddrnandn+Waddrnandp,10.0)
			  + FACwordmetal*ADDRESS_BITS;
	Rwire = FARwordmetal*ADDRESS_BITS*.5;
    tf = Ceq*(Rwire+transreson(Wdummyn,NCH,1)+transreson(Wdummyn,NCH,1));
    *Tag1 = horowitz(nextinputtime,tf,VTHFA2,VTHFA3,FALL);
    nextinputtime = *Tag1/VTHFA3;
	dynPower+=Ceq*VddPow*VddPow*rows*Ntbl;

    /* third stage */

    Ceq = draincap(Waddrnandn,NCH,2)
			  + 2*draincap(Waddrnandp,PCH,1)
			  + gatecap(Wdummyinvn+Wdummyinvp,10.0);
   	tf = Ceq*(transresswitch(Waddrnandp,PCH,1));
   	*Tag2 = horowitz(nextinputtime,tf,VTHFA3,VTHFA4,RISE);
   	nextinputtime = *Tag2/(1-VTHFA4);
	dynPower+=Ceq*VddPow*VddPow*rows*Ntbl;

    /* fourth stage */

    Ceq = (rows)*(gatecap(Wfanorn+Wfanorp,10.0))
			  +draincap(Wdummyinvn,NCH,1)
			  +draincap(Wdummyinvp,PCH,1)
			  + FACbitmetal*rows;
    Rwire = FARbitmetal*rows*.5;
    Req = Rwire+transreson(Wdummyinvn,NCH,1);
    tf = Req*Ceq;
    *Tag3 = horowitz(nextinputtime,tf,VTHFA4,VTHFA5,FALL);
    *outrisetime = *Tag3/VTHFA5;
	dynPower+=Ceq*VddPow*VddPow*Ntbl;

    /* fifth stage */

    Ceq = draincap(Wfanorp,PCH,2)
			  + 2*draincap(Wfanorn,NCH,1)
			  + gatecap(Wfainvn+Wfainvp,10.0);
    tf = Ceq*(transresswitch(Wfanorp,PCH,1) + transreson(Wfanorp,PCH,1));
    *Tag4 = horowitz(nextinputtime,tf,VTHFA5,VTHFA6,RISE);
    nextinputtime = *Tag4/(1-VTHFA6);
	dynPower+=Ceq*VddPow*VddPow;

    /* final stage */

    Ceq = (gatecap(Wdecinvn+Wdecinvp,20.0)+
		       + draincap(Wfainvn,NCH,1)
		       +draincap(Wfainvp,PCH,1));
    Req = transresswitch(Wfainvn,NCH,1);
    tf = Req*Ceq;
    *Tag5 = horowitz(nextinputtime,tf,VTHFA6,VSINV,FALL);
    *outrisetime = *Tag5/VSINV;
	dynPower+=Ceq*VddPow*VddPow;

	// ToDo: Still have to calculate lkgCurrent for this function.. :-)

	power->readOp.dynamic = dynPower*FREQ;
	power->readOp.leakage = lkgCurrent * VddPow;

	power->writeOp.dynamic = dynPower*FREQ;
	power->writeOp.leakage = lkgCurrent * VddPow;

	//	if (Ntbl==32)
	//	  fprintf(stderr," 1st - %f %f\n 2nd - %f %f\n 3rd - %f %f\n 4th - %f %f\n 5th - %f %f\nPD : %f\nNAND : %f\n INV : %f\n NOR : %f\n INV : %f\n", TagdriveA*1e3, TagdriveB*1e3, TagdriveA1*1e3, TagdriveB1*1e3, TagdriveA2*1e3, TagdriveB2*1e3, Tagdrive0a*1e3,Tagdrive0b*1e3,Tagdrive1*1e3, Tagdrive2*1e3, *Tag1*1e3, *Tag2*1e3, *Tag3*1e3, *Tag4*1e3, *Tag5*1e3);
    return(*Tagdrive+*Tag1+*Tag2+*Tag3+*Tag4+*Tag5);
}


/*----------------------------------------------------------------------*/

/* Data array wordline delay (see section 6.2 of tech report) */


double compute_wordline_data(int C,int B,int A,int Ndwl,int Ndbl,int Nspd,double inrisetime,double *outrisetime,powerDef *power)
{
	double Rpdrive,nextrisetime;
    double desiredrisetime,psize,nsize;
    double tf,nextinputtime,Ceq,Req,Rline,Cline;
    int cols, Nact;
    double lkgCurrent=0.0, dynPower=0.0;
    double Tworddrivedel,Twordchargedel;

	int addr_bits=logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd)));

    cols = 8*B*A*Nspd/Ndwl;

    /* Choose a transistor size that makes sense */
    /* Use a first-order approx */

	// Ceq = draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1) +
    //         gatecap(nsize+psize,20.0);

    Ceq = draincap(Wdecinvn,NCH,1) + draincap(Wdecinvp,PCH,1) +
             gatecap(WwlDrvp+WwlDrvn,20.0);

    tf = transreson(Wdecinvp,PCH,1)*Ceq;

    // atleast one wordline driver in each block switches
    if (dualVt == 1)
    	lkgCurrent = 0.5 * (nmos_ileakage(WwlDrvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
					 pmos_ileakage(WwlDrvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox));
	else
    	lkgCurrent = 0.5 * (nmos_ileakage(WwlDrvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
					 pmos_ileakage(WwlDrvp/Leff,VddPow,Vthp,Tkelvin,Tox));

	dynPower+=Ceq*VddPow*VddPow;

    Tworddrivedel = horowitz(inrisetime,tf,VSINV,VSINV,RISE);
    nextinputtime = Tworddrivedel/(1.0-VSINV);

    Cline = (gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             Cwordmetal)*cols+
            draincap(WwlDrvn,NCH,1) + draincap(WwlDrvp,PCH,1);
    Rline = Rwordmetal*cols/2;

    tf = (transreson(WwlDrvp,PCH,1)+Rline)*Cline;

    Twordchargedel = horowitz(nextinputtime,tf,VSINV,VSINV,FALL);
    *outrisetime = Twordchargedel/VSINV;

	dynPower+=Cline*VddPow*VddPow;

	//	fprintf(stderr, "%d %f %f\n", cols, Tworddrivedel*1e3, Twordchargedel*1e3);
	// Number of active blocks in Ndwl modules
	if (Ndwl/Nspd < 1)
		Nact = 1;
	else
		Nact = Ndwl/Nspd;

	power->readOp.dynamic = dynPower*FREQ*Nact;
	power->readOp.leakage = lkgCurrent*Ndwl*Ndbl * VddPow;

	power->writeOp.dynamic = dynPower*Nact*FREQ;
	power->writeOp.leakage = lkgCurrent*Ndwl*Ndbl * VddPow;
	// leakage power for the wordline driver to be accounted in the decoder module..
    return(Tworddrivedel+Twordchargedel);
}


/*----------------------------------------------------------------------*/

/* Tag array wordline delay (see section 6.3 of tech report) */


double compute_wordline_tag_data(int C,int B,int A,int Ndbl,int Nspd,int Ntspd,int Ntwl,int Ntbl,double NSubbanks,double inrisetime,double *outrisetime,powerDef *power)
{
    double tf,m,a,b,c;
    double Cline,Rline,Ceq,nextinputtime;
    int tagbits, Nact;
    double lkgCurrent=0.0, dynPower=0.0;
    double Tworddrivedel,Twordchargedel;
    int cols;
    double Cload;

    /* number of tag bits */
	int addr_bits=logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd)));

    tagbits = ADDRESS_BITS+2-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));
	cols = tagbits * A * Ntspd/Ntwl;

    // capacitive load on the wordline - C_int + C_memCellLoad * NCells
	Cline = (gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
			 gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+ Cwordmetal)*cols;

	Cload = Cline / gatecap(1.0,0.0);
	// apprx width of the driver for optimal delay

	Wtdecinvn = Cload*SizingRatio/3; Wtdecinvp = 2*Cload*SizingRatio/3;

    /* first stage */

    Ceq = draincap(Wtdecinvn,NCH,1) + draincap(Wtdecinvp,PCH,1) +
              gatecap(WtwlDrvn+WtwlDrvp,20.0);
    tf = transreson(Wtdecinvn,NCH,1)*Ceq;

    Tworddrivedel = horowitz(inrisetime,tf,VSINV,VSINV,RISE);
    nextinputtime = Tworddrivedel/(1.0-VSINV);

    dynPower+=Ceq*VddPow*VddPow;

    /* second stage */
    Cline = (gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
             Cwordmetal)*tagbits*A*Ntspd/Ntwl+
            draincap(WtwlDrvn,NCH,1) + draincap(WtwlDrvp,PCH,1);
    Rline = Rwordmetal*tagbits*A*Ntspd/(2*Ntwl);
    tf = (transreson(WtwlDrvp,PCH,1)+Rline)*Cline;

	if (dualVt == 1)
    	lkgCurrent = 0.5 * (nmos_ileakage(WtwlDrvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
					 pmos_ileakage(WtwlDrvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox));
	else
		lkgCurrent = 0.5 * (nmos_ileakage(WtwlDrvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
					 pmos_ileakage(WtwlDrvp/Leff,VddPow,Vthp,Tkelvin,Tox));
    Twordchargedel = horowitz(nextinputtime,tf,VSINV,VSINV,FALL);
    *outrisetime = Twordchargedel/VSINV;

	dynPower+=Cline*VddPow*VddPow;

	// Number of active blocks in Ntwl modules
	if (Ntwl/Ntspd < 1)
		Nact = 1;
	else
		Nact = Ntwl/Ntspd;

	power->readOp.dynamic = dynPower*Nact*FREQ;
	power->readOp.leakage = lkgCurrent *Ntwl*Ntbl* VddPow;

	power->writeOp.dynamic = dynPower*Nact*FREQ;
	power->writeOp.leakage = lkgCurrent *Ntwl*Ntbl* VddPow;

	// leakage power for the wordline drivers to be accounted in the decoder module..
    return(Tworddrivedel+Twordchargedel);

}

/*----------------------------------------------------------------------*/

/* Data array bitline: (see section 6.4 in tech report) */


double compute_bitline_data(int C,int A,int B,int Ndwl,int Ndbl,int Nspd,double inrisetime,double *outrisetime,powerDef *power,double Tpre)
// Tpre: precharge time
{
	double Tbit,Cline,Ccolmux,Rlineb,r1,r2,c1,c2,a,b,c;
   	double m,tstep, Rpdrive;
   	double Cbitrow;    /* bitline capacitance due to access transistor */
   	int rows,cols, Nact;
   	int muxway;
   	double dynWrtPower=0.0, dynRdPower=0.0;
   	double Imem;


	// leakage current of a memory bit-cell
	if (dualVt == 1)
		Imem = nmos_ileakage(Wmemcella/Leff,VddPow,Vt_cell_nmos_high,Tkelvin,Tox) +
				 nmos_ileakage(Wmemcellnmos/Leff,VddPow,Vt_cell_nmos_high,Tkelvin,Tox) +
				 pmos_ileakage(Wmemcellpmos/Leff,VddPow,Vt_cell_pmos_high,Tkelvin,Tox);
	else
		Imem = nmos_ileakage(Wmemcella/Leff,VddPow,Vthn,Tkelvin,Tox) +
				 nmos_ileakage(Wmemcellnmos/Leff,VddPow,Vthn,Tkelvin,Tox) +
				 pmos_ileakage(Wmemcellpmos/Leff,VddPow,Vthp,Tkelvin,Tox);
	// number of bitcells = Cache size in bytes * 8 = C*8
	// leakage current for the whole memory core -
	Imem *= C*8;

   	Cbitrow = draincap(Wmemcella,NCH,1)/2.0; /* due to shared contact */
   	rows = C/(B*A*Ndbl*Nspd);
   	cols = 8*B*A*Nspd/Ndwl;

   	// Determine equivalent width of bitline precharge transistors
   	// we assume that Precharge time = decoder delay + wordlineDriver delay time.
   	// This is because decoder delay + wordline driver delay contributes to most
   	// of the read/write access times.

	Cline = rows*(Cbitrow+Cbitmetal);
	Rpdrive = Tpre/(Cline*log(VSINV)*-1.0);
	Wbitpreequ = restowidth(Rpdrive,PCH);

	// Note that Wbitpreequ is the equiv. pch transistor width
	Wpch = 2.0/3.0*Wbitpreequ;

    if (Wpch > Wpchmax) {
           Wpch = Wpchmax;
	}

	// Isolation transistor width is set to 10 (typical). Its usually wide to reduce
	// resistance offered for transfer of bitline voltage to sense-amp.
	Wiso = 10.0;

	// width of sense precharge
	// (depends on width of sense-amp transistors and output driver size)
	// ToDo: calculate the width of sense-amplifier as a function of access times...

	WsPch = 5.0;   // ToDo: Determine widths of these devices analytically
	WsenseN = 3.0; // sense amplifier N-trans
	WsenseP = 6.0; // sense amplifier P-trans
	WsenseEn = 4.0; // Sense enable transistor of the sense amplifier
	WoBufP = 8.0; // output buffer corresponding to the sense amplifier
	WoBufN = 4.0;

	// ToDo: detemine the size of colmux (Wbitmux)

   	if (8*B/BITOUT == 1 && Ndbl*Nspd==1) {
   		Cline = rows*(Cbitrow+Cbitmetal)+2*draincap(Wbitpreequ,PCH,1);
        Ccolmux = gatecap(WsenseN+WsenseP,10.0);
        Rlineb = Rbitmetal*rows/2.0;
        r1 = Rlineb;
	    // muxover=1;
	} else {
		if (Ndbl*Nspd > MAX_COL_MUX)
	    {
	      	// muxover=8*B/BITOUT;
			muxway=MAX_COL_MUX;
	    }
		else if (8*B*Ndbl*Nspd/BITOUT > MAX_COL_MUX)
	    {
			muxway=MAX_COL_MUX;
	    	// muxover=(8*B/BITOUT)/(MAX_COL_MUX/(Ndbl*Nspd));
	    }
	    else
	    {
			muxway=8*B*Nspd*Ndbl/BITOUT;
			// muxover=1;
	    }

    	Cline = rows*(Cbitrow+Cbitmetal) + 2*draincap(Wbitpreequ,PCH,1) +
              draincap(Wiso,PCH,1);
    	Ccolmux = muxway*(draincap(Wiso,PCH,1))+gatecap(WsenseN+WsenseP,10.0);
    	Rlineb = Rbitmetal*rows/2.0;
    	r1 = Rlineb + transreson(Wiso,PCH,1);
	 }

    r2 = transreson(Wmemcella,NCH,1) +
             transreson(Wmemcella*Wmemcellbscale,NCH,1);
    c1 = Ccolmux;
    c2 = Cline;

	// Number of active blocks in Ntwl modules during a read op
	if (Ndwl/Nspd < 1)
		Nact = 1;
	else
		Nact = Ndwl/Nspd;

	dynRdPower+=c1*VddPow*VddPow*BITOUT*A*muxover;
	dynRdPower+=c2*VddPow*VbitprePow*cols*Nact;

	if (cols > BITIN)
	{
		dynWrtPower += c2*VddPow*VddPow*BITIN;
		dynWrtPower += c2*VddPow*VbitprePow*(cols-BITIN);
	}
	else
		dynWrtPower += c2*VddPow*VddPow*BITIN;


	//fprintf(stderr, "Pow %f %f\n", c1*VddPow*VddPow*BITOUT*A*muxover*1e3, c2*VddPow*VbitprePow*cols*1e3);
    tstep = (r2*c2+(r1+r2)*c1)*log((Vbitpre)/(Vbitpre-Vbitsense));

    /* take input rise time into account */

    m = Vdd/inrisetime;
    if (tstep <= (0.5*(Vdd-Vt)/m)) {
   		a = m;
        b = 2*((Vdd*0.5)-Vt);
        c = -2*tstep*(Vdd-Vt)+1/m*((Vdd*0.5)-Vt)*((Vdd*0.5)-Vt);
        Tbit = (-b+sqrt(b*b-4*a*c))/(2*a);
    } else {
      	Tbit = tstep + (Vdd+Vt)/(2*m) - (Vdd*0.5)/m;
   	}

    *outrisetime = Tbit/(log((Vbitpre-Vbitsense)/Vdd));

    power->writeOp.dynamic = dynWrtPower*FREQ;
    power->writeOp.leakage = Imem * VddPow;

	power->readOp.dynamic = dynRdPower*FREQ;
    power->readOp.leakage = Imem * VddPow;

    return(Tbit);
}

/* It is assumed the sense amps have a constant delay
   (see section 6.5) */

double compute_sense_amp_data(int C,int A,int B,int Ndwl,int Ndbl,int Nspd, double inrisetime,double *outrisetime, powerDef *power)
{
	double Cload;
	int cols,Nact;
	double IsenseEn, IsenseN, IsenseP, IoBufP, IoBufN, Iiso, Ipch, IsPch;
	double lkgIdlePh, lkgReadPh, lkgWritePh;
	double lkgRead, lkgWrite, lkgIdle;

 	cols = 8*B*A*Nspd/Ndwl;

	// Number of active blocks in Ntwl modules during a read op
	if (Ndwl/Nspd < 1)
		Nact = 1;
	else
		Nact = Ndwl/Nspd;

	if (dualVt == 1)
	{
		IsenseEn = nmos_ileakage(WsenseEn/Leff,VddPow,Vt_cell_nmos_high,Tkelvin,Tox);
		IsenseN = nmos_ileakage(WsenseN/Leff,VddPow,Vt_cell_nmos_high,Tkelvin,Tox);
		IsenseP = pmos_ileakage(WsenseP/Leff,VddPow,Vt_cell_pmos_high,Tkelvin,Tox);
		IoBufN = nmos_ileakage(WoBufN/Leff,VddPow,Vt_cell_nmos_high,Tkelvin,Tox);
		IoBufP = pmos_ileakage(WoBufP/Leff,VddPow,Vt_cell_pmos_high,Tkelvin,Tox);
		Iiso = nmos_ileakage(Wiso/Leff,VddPow,Vt_cell_nmos_high,Tkelvin,Tox);
		Ipch = pmos_ileakage(Wpch/Leff,VddPow,Vt_cell_pmos_high,Tkelvin,Tox);
		IsPch = pmos_ileakage(WsPch/Leff,VddPow,Vt_cell_pmos_high,Tkelvin,Tox);
	}
	else
	{
		IsenseEn = nmos_ileakage(WsenseEn/Leff,VddPow,Vthn,Tkelvin,Tox);
		IsenseN = nmos_ileakage(WsenseN/Leff,VddPow,Vthn,Tkelvin,Tox);
		IsenseP = pmos_ileakage(WsenseP/Leff,VddPow,Vthp,Tkelvin,Tox);
		IoBufN = nmos_ileakage(WoBufN/Leff,VddPow,Vthn,Tkelvin,Tox);
		IoBufP = pmos_ileakage(WoBufP/Leff,VddPow,Vthp,Tkelvin,Tox);
		Iiso = nmos_ileakage(Wiso/Leff,VddPow,Vthn,Tkelvin,Tox);
		Ipch = pmos_ileakage(Wpch/Leff,VddPow,Vthp,Tkelvin,Tox);
		IsPch = pmos_ileakage(WsPch/Leff,VddPow,Vthp,Tkelvin,Tox);
	}

	lkgIdlePh = IsenseEn + 2*IoBufP;
	lkgWritePh = 2*Ipch + Iiso + IsenseEn + 2*IoBufP;
	lkgReadPh = 2*IsPch + Iiso + IsenseN + IsenseP + IoBufN + IoBufP;

	// read cols in the inactive blocks would be in idle phase
	lkgRead = lkgReadPh * cols * Nact + lkgIdlePh * cols * (Nact - 1);
	// only the cols in which data is written into are in write ph
	// all the remaining cols are considered to be in idle phase
	lkgWrite = lkgWritePh * BITIN + lkgIdlePh * (cols*Ndwl - BITIN);
	lkgIdle = lkgIdlePh * cols * Ndwl;

	// sense amplifier has to drive logic in "data out driver" and sense precharge load.
	// load seen by sense amp

	Cload = gatecap(WsenseP+WsenseN,5.0) + draincap(WsPch,PCH,1) +
			gatecap(Woutdrvnandn+Woutdrvnandp,1.0) + gatecap(Woutdrvnorn+Woutdrvnorp,1.0);

   	*outrisetime = tfalldata;
   	power->readOp.dynamic = 0.5* Cload * VddPow * VddPow *BITOUT * A * muxover * FREQ;
   	power->readOp.leakage = ((lkgRead + lkgIdle)/2 ) * VddPow;

   	power->writeOp.dynamic = 0.0;
	power->writeOp.leakage = ((lkgWrite + lkgIdle)/2 ) * VddPow;

   	return(tsensedata+2.5e-10);
}

/*--------------------------------------------------------------*/
/* Tag array bitline: (see section 6.4 in tech report) */

double compute_bitline_tag_data(int C,int A,int B,int Ntwl,int Ntbl,int Ntspd,double NSubbanks,double inrisetime,double *outrisetime,powerDef *power,double Tpre)
{
	double Tbit,Cline,Ccolmux,Rlineb,r1,r2,c1,c2,a,b,c;
    double m,tstep, Rpdrive;
    double Cbitrow;    /* bitline capacitance due to access transistor */
    int tagbits;
    int rows,cols,Nact;
    double lkgCurrent=0.0, dynRdPower=0.0, dynWrtPower=0.0;;


	// leakage current of a memory bit-cell
	if (dualVt == 1)
		lkgCurrent = nmos_ileakage(Wmemcella/Leff,VddPow,Vt_cell_nmos_high,Tkelvin,Tox) +
				 nmos_ileakage(Wmemcellnmos/Leff,VddPow,Vt_cell_nmos_high,Tkelvin,Tox) +
				 pmos_ileakage(Wmemcellpmos/Leff,VddPow,Vt_cell_pmos_high,Tkelvin,Tox);
	else
		lkgCurrent = nmos_ileakage(Wmemcella/Leff,VddPow,Vthn,Tkelvin,Tox) +
				 nmos_ileakage(Wmemcellnmos/Leff,VddPow,Vthn,Tkelvin,Tox) +
				 pmos_ileakage(Wmemcellpmos/Leff,VddPow,Vthp,Tkelvin,Tox);

    tagbits = ADDRESS_BITS+2-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));

    Cbitrow = draincap(Wmemcella,NCH,1)/2.0; /* due to shared contact */
    rows = C/(B*A*Ntbl*Ntspd);
    cols = tagbits*A*Ntspd/Ntwl;

	// leakage current for the the whole memory core -
	lkgCurrent *= C/B*tagbits;

	// Determine equivalent width of bitline precharge transistors
   	// we assume that Precharge time = decoder delay + wordlineDriver delay time.
   	// This is because decoder delay + wordline driver delay contributes to most
   	// of the read/write access times.

	Cline = rows*(Cbitrow+Cbitmetal);
	Rpdrive = Tpre/(Cline*log(VSINV)*-1.0);
	Wtbitpreequ = restowidth(Rpdrive,PCH);

	// Note that Wbitpreequ is the equiv. pch transistor width
    Wtpch = 2.0/3.0*Wtbitpreequ;

    if (Wtpch > Wpchmax) {
           Wtpch = Wpchmax;
    }

	// width of sense precharge
	// (depends on width of sense-amp transistors and output driver size)
	// ToDo: calculate the width of sense-amplifier as a function of access times...

	WtsPch = 5.0;   // ToDo: Determine widths of these devices analytically
	WtsenseN = 3.0; // sense amplifier N-trans
	WtsenseP = 6.0; // sense amplifier P-trans
	WtsenseEn = 4.0; // Sense enable transistor of the sense amplifier
	WtoBufP = 8.0; // output buffer corresponding to the sense amplifier
	WtoBufN = 4.0;
    // Isolation transistor width is set to 10 (typical). Its usually wide to reduce
    // resistance offered for transfer of bitline voltage to sense-amp.
    Wtiso = 10.0;

    // ToDo: detemine the size of colmux (Wtbitmux)

    if (Ntbl*Ntspd == 1) {
 	   Cline = rows*(Cbitrow+Cbitmetal)+2*draincap(Wtbitpreequ,PCH,1);
       Ccolmux = gatecap(WtsenseN+WtsenseP,10.0);
       Rlineb = Rbitmetal*rows/2.0;
       r1 = Rlineb;
	} else {
       Cline = rows*(Cbitrow+Cbitmetal) + 2*draincap(Wtbitpreequ,PCH,1) +
                 draincap(Wtiso,PCH,1);
       Ccolmux = Ntspd*Ntbl*(draincap(Wtiso,PCH,1))+gatecap(WtsenseN+WtsenseP,10.0);
       Rlineb = Rbitmetal*rows/2.0;
 	   r1 = Rlineb + transreson(Wtiso,PCH,1);
	}

	r2 = transreson(Wmemcella,NCH,1) +
           transreson(Wmemcella*Wmemcellbscale,NCH,1);

    c1 = Ccolmux;
    c2 = Cline;

	// tagbits * A are read out from tag array
	dynRdPower+=c1*VddPow*VddPow*tagbits * A;

	// Number of active Ntwl blocks
	if (Ntwl/Ntspd < 1)
		Nact = 1;
	else
		Nact = Ntwl/Ntspd;

	dynRdPower+=c2*VddPow*VbitprePow*cols*Nact;

	if (cols > tagbits)
	{
		dynWrtPower += c2*VddPow*VddPow*tagbits;
		dynWrtPower += c2*VddPow*VbitprePow*(cols-tagbits);
	}
	else
		dynWrtPower += c2*VddPow*VddPow*tagbits;

	// fprintf(stderr, "Pow %f %f\n", c1*VddPow*VddPow*1e3, c2*VddPow*VbitprePow*cols*1e3);

    tstep = (r2*c2+(r1+r2)*c1)*log((Vbitpre)/(Vbitpre-Vbitsense));

    /* take into account input rise time */

    m = Vdd/inrisetime;
    if (tstep <= (0.5*(Vdd-Vt)/m)) {
        a = m;
        b = 2*((Vdd*0.5)-Vt);
        c = -2*tstep*(Vdd-Vt)+1/m*((Vdd*0.5)-Vt)*((Vdd*0.5)-Vt);
        Tbit = (-b+sqrt(b*b-4*a*c))/(2*a);
    } else {
        Tbit = tstep + (Vdd+Vt)/(2*m) - (Vdd*0.5)/m;
    }

    *outrisetime = Tbit/(log((Vbitpre-Vbitsense)/Vdd));

    power->readOp.dynamic = dynRdPower*FREQ;
	power->readOp.leakage = lkgCurrent * VddPow;

	power->writeOp.dynamic = dynWrtPower*FREQ;
	power->writeOp.leakage = lkgCurrent * VddPow;

	return(Tbit);
}

double compute_sense_amp_tag_data(int C,int A,int B,int Ntwl,int Ntbl,int Ntspd,double NSubbanks,double inrisetime, double *outrisetime, powerDef *power)
{
	double Cload;
	int cols, tagbits, Nact;
	double IsenseEn, IsenseN, IsenseP, IoBufP, IoBufN, Iiso, Ipch, IsPch;
	double lkgIdlePh, lkgReadPh, lkgWritePh;
	double lkgRead, lkgWrite, lkgIdle;

	tagbits = ADDRESS_BITS+2-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));
	cols = tagbits*A*Ntspd/Ntwl;

	// Number of active blocks in Ntwl modules during a read op
	if (Ntwl/Ntspd < 1)
		Nact = 1;
	else
		Nact = Ntwl/Ntspd;

	if (dualVt == 1)
	{
		IsenseEn = nmos_ileakage(WtsenseEn/Leff,VddPow,Vt_cell_nmos_high,Tkelvin,Tox);
		IsenseN = nmos_ileakage(WtsenseN/Leff,VddPow,Vt_cell_nmos_high,Tkelvin,Tox);
		IsenseP = pmos_ileakage(WtsenseP/Leff,VddPow,Vt_cell_pmos_high,Tkelvin,Tox);
		IoBufN = nmos_ileakage(WtoBufN/Leff,VddPow,Vt_cell_nmos_high,Tkelvin,Tox);
		IoBufP = pmos_ileakage(WtoBufP/Leff,VddPow,Vt_cell_pmos_high,Tkelvin,Tox);
		Iiso = nmos_ileakage(Wtiso/Leff,VddPow,Vt_cell_nmos_high,Tkelvin,Tox);
		Ipch = pmos_ileakage(Wtpch/Leff,VddPow,Vt_cell_pmos_high,Tkelvin,Tox);
		IsPch = pmos_ileakage(WtsPch/Leff,VddPow,Vt_cell_pmos_high,Tkelvin,Tox);
	}
	else
	{
		IsenseEn = nmos_ileakage(WtsenseEn/Leff,VddPow,Vthn,Tkelvin,Tox);
		IsenseN = nmos_ileakage(WtsenseN/Leff,VddPow,Vthn,Tkelvin,Tox);
		IsenseP = pmos_ileakage(WtsenseP/Leff,VddPow,Vthp,Tkelvin,Tox);
		IoBufN = nmos_ileakage(WtoBufN/Leff,VddPow,Vthn,Tkelvin,Tox);
		IoBufP = pmos_ileakage(WtoBufP/Leff,VddPow,Vthp,Tkelvin,Tox);
		Iiso = nmos_ileakage(Wtiso/Leff,VddPow,Vthn,Tkelvin,Tox);
		Ipch = pmos_ileakage(Wtpch/Leff,VddPow,Vthp,Tkelvin,Tox);
		IsPch = pmos_ileakage(WtsPch/Leff,VddPow,Vthp,Tkelvin,Tox);
	}

	lkgIdlePh = IsenseEn + 2*IoBufP;
	lkgWritePh = 2*Ipch + Iiso + IsenseEn + 2*IoBufP;
	lkgReadPh = 2*IsPch + Iiso + IsenseN + IsenseP + IoBufN + IoBufP;

	// read cols in the inactive blocks would be in idle phase
	lkgRead = lkgReadPh * cols * Nact + lkgIdlePh * cols * (Nact - 1);
	// only the cols in which data is written into are in write ph
	// all the remaining cols are considered to be in idle phase
	lkgWrite = lkgWritePh * tagbits + lkgIdlePh * (cols*Ntwl - tagbits);
	lkgIdle = lkgIdlePh * cols * Ntwl;

	// sense amplifier has to drive logic in "data out driver" and sense precharge load.
	// load seen by sense amp

	Cload = gatecap(WtsenseP+WtsenseN,5.0) + draincap(WtsPch,PCH,1);
				//gatecap(Woutdrvnandn+Woutdrvnandp,1.0) + gatecap(Woutdrvnorn+Woutdrvnorp,1.0);

	*outrisetime = tfalltag;
	power->readOp.dynamic = 0.5* Cload * VddPow * VddPow *tagbits * A * FREQ;
	power->readOp.leakage = ((lkgRead + lkgIdle)/2 ) * VddPow;

	power->writeOp.dynamic = 0.0;
	power->writeOp.leakage = ((lkgWrite + lkgIdle)/2 ) * VddPow;

   	return(tsensetag+2.5e-10);
}

/*----------------------------------------------------------------------*/


void compute_write_control_data(int C,int A,int B,int Ndwl,int Ndbl,int Nspd,double NSubbanks,powerDef *power)
{
	double lkgCurrent;
	double Ctotal,Cload; // to calculate dynamic power

	// the power contributions come from the logic generating
	// write col multiplexer selects.
	double cols = 8*B*A*Nspd/Ndwl ;
	double Cline = cols * Cbitmetal;

	int Wcolmux = 4.0;
	int colMuxSize = A*Nspd;

	power->readOp.dynamic = 0.0;
	power->readOp.leakage = 0.0;

	if (A*Nspd == 1) return ;

	// typical width of col mux device, Wcolmux = 4

	// cap load seen by each col select
	Cload = Cline + (cols/colMuxSize) * gatecap(Wcolmux, 0.0);
	WwrtMuxSelDrvn = (Cload / gatecap(1.0,0.0))*SizingRatio/3;
	WwrtMuxSelDrvp = (Cload / gatecap(1.0,0.0))*SizingRatio*2/3;

	if (dualVt == 1)
		lkgCurrent = 0.5 * (nmos_ileakage(WwrtMuxSelDrvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			  pmos_ileakage(WwrtMuxSelDrvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent = 0.5 * (nmos_ileakage(WwrtMuxSelDrvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			  pmos_ileakage(WwrtMuxSelDrvp/Leff,VddPow,Vthp,Tkelvin,Tox) );

	// lkgCurrent for all mux sel drivers
	lkgCurrent *= colMuxSize;

	// lkgCurrent for the logic generating the mux drivers
	// based on experiments, total leakage ~= 1.4 times buffer leakage

	lkgCurrent *= 1.4;

	power->readOp.leakage = lkgCurrent * VddPow * Ndwl * Ndbl;
	power->readOp.dynamic = 0.0;

	power->writeOp.leakage = lkgCurrent * VddPow * Ndwl * Ndbl;
	power->writeOp.dynamic = 1.4* Cload * VddPow * VddPow*FREQ;
}

void compute_tag_write_control_data(int C,int A,int B,int Ntwl,int Ntbl,int Ntspd,double NSubbanks, powerDef *power)
{
	double lkgCurrent;
	double Ctotal,Cload; // to calculate dynamic power

	// the power contributions come from the logic generating
	// write col multiplexer selects.
	int tagbits = ADDRESS_BITS+2 - (int)logtwo((double)C) + (int)logtwo((double)A)-(int)(logtwo(NSubbanks));
	int cols = tagbits * A * Ntspd/Ntwl;

	double Cline = cols * Cbitmetal;

	int Wcolmux = 4.0;
	int colMuxSize = A*Ntspd;

	power->readOp.dynamic = 0.0;
	power->readOp.leakage = 0.0;

	if (A*Ntspd == 1) return ;

	// typical width of col mux device, Wcolmux = 4

	// cap load seen by each col select
	Cload = Cline + (cols/colMuxSize) * gatecap(Wcolmux, 0.0);
	WtwrtMuxSelDrvn = (Cload / gatecap(1.0,0.0))*SizingRatio/3;
	WtwrtMuxSelDrvp = (Cload / gatecap(1.0,0.0))*SizingRatio*2/3;

	if (dualVt == 1)
		lkgCurrent = 0.5 * (nmos_ileakage(WtwrtMuxSelDrvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			  pmos_ileakage(WtwrtMuxSelDrvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent = 0.5 * (nmos_ileakage(WtwrtMuxSelDrvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			  pmos_ileakage(WtwrtMuxSelDrvp/Leff,VddPow,Vthp,Tkelvin,Tox) );

	// lkgCurrent for all mux sel drivers
	lkgCurrent *= colMuxSize;

	// lkgCurrent for the logic generating the mux drivers
	// based on experiments, total leakage ~= 1.4 times buffer leakage

	lkgCurrent *= 1.4;

	power->readOp.leakage = lkgCurrent * Ntbl * Ntwl * VddPow;
	power->readOp.dynamic = 0.0;

	power->writeOp.leakage = lkgCurrent * Ntbl * Ntwl * VddPow;
	power->writeOp.dynamic = 1.4 * Cload * VddPow * VddPow*FREQ;
}


void compute_read_control_data(int C,int A,int B,int Ndwl,int Ndbl,int Nspd,double NSubbanks,powerDef *power)
{
	double lkgCurrent;
	double Ctotal; // to calculate dynamic power

	// leakage current in read control is contributed by the drivers
	// for precharge, isolation, sense-amp, and sense-pch controls.

	// no. of sense amplifiers = BITOUT;
	// no. of isolation transistors (each bitline pair has 2) = 2*8*block_size*nspd*ndbl
	// no. of precharge groups = 8*block_size*nspd*ndbl
	// no. of sense precharge groups = BITOUT;

	double cols = 8*B*A*Nspd/Ndwl ;
	double Cline = cols * Cbitmetal;

	// effective precharge width for precharge devices
	double Weff = Wpch * cols * Ndwl * Ndbl;
	// effective load cap to be driven by pch buffers
	double Ceff = gatecap(Weff, 0.0) + Cline;
	//effective size of driver driving pch transistors
	double WeffDrv = Ceff*SizingRatio/gatecap(1.0, 0.0);

	// Width of precharge drivers
	WpchDrvn = WeffDrv*SizingRatio/3;
	WpchDrvp = 2*WeffDrv*SizingRatio/3;

	Ctotal = Ceff;

	// effective width of isolation devices
	Weff = Wiso * cols * Ndwl * Ndbl;
	// effective load cap to be driven by ISO buffers
	Ceff = gatecap(Weff, 0.0) + Cline;
	//effective size of driver driving ISO transistors
	WeffDrv = Ceff*SizingRatio/gatecap(1.0, 0.0);

	// Width of isolation drivers
	WisoDrvn = WeffDrv*SizingRatio/3;
	WisoDrvp = 2*WeffDrv*SizingRatio/3;

	Ctotal += Ceff;

	// effective width for sense precharge devices
	Weff = WsPch * BITOUT * A;
	// effective load cap to be driven by sPch buffers
	Ceff = gatecap(Weff, 0.0) + Cline;
	//effective size of driver driving sPch transistors
	WeffDrv = Ceff*SizingRatio/gatecap(1.0, 0.0);

	// Width of isolation drivers
	WspchDrvn = WeffDrv*SizingRatio/3;
	WspchDrvp = 2*WeffDrv*SizingRatio/3;

	Ctotal += Ceff;

	// effective width for sense enable devices
	Weff = WsenseEn * BITOUT * A;
	// effective load cap to be driven by senseEn buffers
	Ceff = gatecap(Weff, 0.0) + Cline;
	//effective size of driver driving senseEn transistors
	WeffDrv = Ceff*SizingRatio/gatecap(1.0, 0.0);

	// Width of isolation drivers
	WsenseEnDrvn = WeffDrv*SizingRatio/3;
	WsenseEnDrvp = 2*WeffDrv*SizingRatio/3;

	Ctotal += Ceff;

	// leakage of the read control drivers..
	if (dualVt == 1) {
		lkgCurrent = 0.5 * (nmos_ileakage(WpchDrvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
				pmos_ileakage(WpchDrvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

		lkgCurrent += 0.5 * (nmos_ileakage(WisoDrvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			  	pmos_ileakage(WisoDrvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

		lkgCurrent += 0.5 * (nmos_ileakage(WspchDrvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			  	pmos_ileakage(WspchDrvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

		lkgCurrent += 0.5 * (nmos_ileakage(WsenseEnDrvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			  	pmos_ileakage(WsenseEnDrvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

	}
	else {
		lkgCurrent = 0.5 * (nmos_ileakage(WpchDrvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			  	pmos_ileakage(WpchDrvp/Leff,VddPow,Vthp,Tkelvin,Tox) );

		lkgCurrent += 0.5 * (nmos_ileakage(WisoDrvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
				pmos_ileakage(WisoDrvp/Leff,VddPow,Vthp,Tkelvin,Tox) );

		lkgCurrent += 0.5 * (nmos_ileakage(WspchDrvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
				pmos_ileakage(WspchDrvp/Leff,VddPow,Vthp,Tkelvin,Tox) );

		lkgCurrent += 0.5 * (nmos_ileakage(WsenseEnDrvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
				pmos_ileakage(WsenseEnDrvp/Leff,VddPow,Vthp,Tkelvin,Tox) );
	}
	// based on experiments leakage power in the whole read/write control
	// is ~1.4 times the leakage in drivers.

	lkgCurrent *= 1.4;

	power->readOp.leakage = lkgCurrent * VddPow;
	power->readOp.dynamic = Ctotal*VddPow*VddPow*FREQ;

	power->writeOp.leakage = lkgCurrent * VddPow;
	power->writeOp.dynamic = 0.0;


}

void compute_tag_read_control_data(int C,int A,int B,int Ntwl,int Ntbl,int Ntspd,double NSubbanks,powerDef *power)
{
	double lkgCurrent;
	double Ctotal; // to calculate dynamic power

	// leakage current in read control is contributed by the drivers
	// for precharge, isolation, sense-amp, and sense-pch controls.

	// no. of sense amplifiers = BITOUT;
	// no. of isolation transistors (each bitline pair has 2) = 2*8*block_size*nspd*ndbl
	// no. of precharge groups = 8*block_size*nspd*ndbl
	// no. of sense precharge groups = BITOUT;

	int tagbits = ADDRESS_BITS+2 - (int)logtwo((double)C) + (int)logtwo((double)A)-(int)(logtwo(NSubbanks));
	int cols = tagbits * A * Ntspd/Ntwl;

	double Cline = cols * Cbitmetal;

	// effective precharge width for precharge devices
	double Weff = Wtpch * cols * Ntwl * Ntbl;
	// effective load cap to be driven by pch buffers
	double Ceff = gatecap(Weff, 0.0) + Cline;
	//effective size of driver driving pch transistors
	double WeffDrv = Ceff*SizingRatio/gatecap(1.0, 0.0);

	// Width of precharge drivers
	WtpchDrvn = WeffDrv*SizingRatio/3;
	WtpchDrvp = 2*WeffDrv*SizingRatio/3;

	Ctotal = Ceff;

	// effective width of isolation devices
	Weff = Wtiso * cols * Ntwl * Ntbl;
	// effective load cap to be driven by ISO buffers
	Ceff = gatecap(Weff, 0.0) + Cline;
	//effective size of driver driving ISO transistors
	WeffDrv = Ceff*SizingRatio/gatecap(1.0, 0.0);

	// Width of isolation drivers
	WtisoDrvn = WeffDrv*SizingRatio/3;
	WtisoDrvp = 2*WeffDrv*SizingRatio/3;

	Ctotal += Ceff;

	// effective width for sense precharge devices
	Weff = WtsPch * tagbits * A;
	// effective load cap to be driven by sPch buffers
	Ceff = gatecap(Weff, 0.0) + Cline;
	//effective size of driver driving sPch transistors
	WeffDrv = Ceff*SizingRatio/gatecap(1.0, 0.0);

	// Width of isolation drivers
	WtspchDrvn = WeffDrv*SizingRatio/3;
	WtspchDrvp = 2*WeffDrv*SizingRatio/3;

	Ctotal += Ceff;

	// effective width for sense enable devices
	Weff = WtsenseEn * tagbits * A;
	// effective load cap to be driven by senseEn buffers
	Ceff = gatecap(Weff, 0.0) + Cline;
	//effective size of driver driving senseEn transistors
	WeffDrv = Ceff*SizingRatio/gatecap(1.0, 0.0);

	// Width of isolation drivers
	WtsenseEnDrvn = WeffDrv*SizingRatio/3;
	WtsenseEnDrvp = 2*WeffDrv*SizingRatio/3;

	Ctotal += Ceff;

	// leakage of the read control drivers..
	if (dualVt == 1) {
		lkgCurrent = 0.5 * (nmos_ileakage(WtpchDrvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			  pmos_ileakage(WtpchDrvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

		lkgCurrent += 0.5 * (nmos_ileakage(WtisoDrvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			  pmos_ileakage(WtisoDrvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

		lkgCurrent += 0.5 * (nmos_ileakage(WtspchDrvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			  pmos_ileakage(WtspchDrvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

		lkgCurrent += 0.5 * (nmos_ileakage(WtsenseEnDrvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			  pmos_ileakage(WtsenseEnDrvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	}
	else  {
		lkgCurrent = 0.5 * (nmos_ileakage(WtpchDrvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			  pmos_ileakage(WtpchDrvp/Leff,VddPow,Vthp,Tkelvin,Tox) );

		lkgCurrent += 0.5 * (nmos_ileakage(WtisoDrvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			  pmos_ileakage(WtisoDrvp/Leff,VddPow,Vthp,Tkelvin,Tox) );

		lkgCurrent += 0.5 * (nmos_ileakage(WtspchDrvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			  pmos_ileakage(WtspchDrvp/Leff,VddPow,Vthp,Tkelvin,Tox) );

		lkgCurrent += 0.5 * (nmos_ileakage(WtsenseEnDrvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			  pmos_ileakage(WtsenseEnDrvp/Leff,VddPow,Vthp,Tkelvin,Tox) );
	}

	// based on experiments leakage power in the whole read/write control
	// is ~1.4 times the leakage in drivers.

	lkgCurrent *= 1.4;

	power->readOp.leakage = lkgCurrent * VddPow;
	power->readOp.dynamic = Ctotal*VddPow*VddPow*FREQ;

	power->writeOp.leakage = lkgCurrent * VddPow ;
	power->writeOp.dynamic = 0.0;

}


/*----------------------------------------------------------------------*/

/* Comparator Delay (see section 6.6)

double compute_compare_time(int C,int A,int Ntbl,int Ntspd,double NSubbanks,double inputtime,double *outputtime)
{
	double Req,Ceq,tf,st1del,st2del,st3del,nextinputtime,m;
	double c1,c2,r1,r2,tstep,a,b,c;
   	double Tcomparatorni;
   	double lkgCurrent;
   	int cols,tagbits;

   	//* First Inverter

   	Ceq = gatecap(Wcompinvn2+Wcompinvp2,10.0) +
         draincap(Wcompinvp1,PCH,1) + draincap(Wcompinvn1,NCH,1);
   	Req = transreson(Wcompinvp1,PCH,1);
   	tf = Req*Ceq;
   	st1del = horowitz(inputtime,tf,VTHCOMPINV,VTHCOMPINV,FALL);
   	nextinputtime = st1del/VTHCOMPINV;

	if (dualVt == 1)
		lkgCurrent = 0.5 * (
			nmos_ileakage(Wcompinvn1/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wcompinvp1/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent = 0.5 * (
			nmos_ileakage(Wcompinvn1/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wcompinvp1/Leff,VddPow,Vthp,Tkelvin,Tox) );
   	//* Second Inverter

   	Ceq = gatecap(Wcompinvn3+Wcompinvp3,10.0) +
         draincap(Wcompinvp2,PCH,1) + draincap(Wcompinvn2,NCH,1);
   	Req = transreson(Wcompinvn2,NCH,1);
   	tf = Req*Ceq;
   	st2del = horowitz(inputtime,tf,VTHCOMPINV,VTHCOMPINV,RISE);
   	nextinputtime = st1del/(1.0-VTHCOMPINV);

	if (dualVt == 1)
		lkgCurrent += 0.5 * (
			nmos_ileakage(Wcompinvn2/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wcompinvp2/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += 0.5 * (
			nmos_ileakage(Wcompinvn2/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wcompinvp2/Leff,VddPow,Vthp,Tkelvin,Tox) );
   //* Third Inverter

   	Ceq = gatecap(Wevalinvn+Wevalinvp,10.0) +
   	      draincap(Wcompinvp3,PCH,1) + draincap(Wcompinvn3,NCH,1);
   	Req = transreson(Wcompinvp3,PCH,1);
   	tf = Req*Ceq;
   	st3del = horowitz(nextinputtime,tf,VTHCOMPINV,VTHEVALINV,FALL);
   	nextinputtime = st1del/(VTHEVALINV);

	if (dualVt == 1)
		lkgCurrent += 0.5 * (
			nmos_ileakage(Wcompinvn3/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wcompinvp3/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += 0.5 * (
			nmos_ileakage(Wcompinvn3/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wcompinvp3/Leff,VddPow,Vthp,Tkelvin,Tox) );

   	//* Final Inverter (virtual ground driver) discharging compare part

   	tagbits = ADDRESS_BITS+2 - (int)logtwo((double)C) + (int)logtwo((double)A)-(int)(logtwo(NSubbanks));
   	cols = tagbits*Ntbl*Ntspd;

   	r1 = transreson(Wcompn,NCH,2);
   	r2 = transreson(Wevalinvn,NCH,1);  //* was switch
   	c2 = (tagbits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))+
   	      draincap(Wevalinvp,PCH,1) + draincap(Wevalinvn,NCH,1);
   	c1 = (tagbits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))
   	     +draincap(Wcompp,PCH,1) + gatecap(Wmuxdrv12n+Wmuxdrv12p,20.0) +
   	     cols*Cwordmetal;

	if (dualVt == 1) {
		lkgCurrent += 0.5 * (
			nmos_ileakage(Wevalinvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wevalinvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
		// the comparator compares cols number of bits. Since these transistors are
		// stacked, a stacking factor of 0.2 is used
		lkgCurrent += cols * 0.2 * 0.5 * (
			nmos_ileakage(Wcompn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wcompp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

		// leakage due to the mux driver
		lkgCurrent += 0.5 * (
			nmos_ileakage(Wmuxdrv12n/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wmuxdrv12p/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	}
	else {
		lkgCurrent += 0.5 * (
			nmos_ileakage(Wevalinvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wevalinvp/Leff,VddPow,Vthp,Tkelvin,Tox) );
		// the comparator compares cols number of bits. Since these transistors are
		// stacked, a stacking factor of 0.2 is used
		lkgCurrent += cols * 0.2 * 0.5 * (
			nmos_ileakage(Wcompn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wcompp/Leff,VddPow,Vthp,Tkelvin,Tox) );

		// leakage due to the mux driver
		lkgCurrent += 0.5 * (
			nmos_ileakage(Wmuxdrv12n/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wmuxdrv12p/Leff,VddPow,Vthp,Tkelvin,Tox) );
	}
   	//* time to go to threshold of mux driver
   	tstep = (r2*c2+(r1+r2)*c1)*log(1.0/VTHMUXDRV1);

   	//* take into account non-zero input rise time

   	m = Vdd/nextinputtime;

   	if ((tstep) <= (0.5*(Vdd-Vt)/m)) {
  		a = m;
	    b = 2*((Vdd*VTHEVALINV)-Vt);
        c = -2*(tstep)*(Vdd-Vt)+1/m*((Vdd*VTHEVALINV)-Vt)*((Vdd*VTHEVALINV)-Vt);
 	    Tcomparatorni = (-b+sqrt(b*b-4*a*c))/(2*a);
   	} else {
	    Tcomparatorni = (tstep) + (Vdd+Vt)/(2*m) - (Vdd*VTHEVALINV)/m;
   	}
   	*outputtime = Tcomparatorni/(1.0-VTHMUXDRV1);

   	return(Tcomparatorni+st1del+st2del+st3del);
}
*/

double compute_half_compare_data(int C,int A,int Ntbl,int Ntspd,double NSubbanks,double inputtime,double *outputtime,powerDef *power)
{
   	double Req,Ceq,tf,st1del,st2del,st3del,nextinputtime,m;
   	double c1,c2,r1,r2,tstep,a,b,c;
   	double Tcomparatorni;
   	int cols,tagbits;
   	double lkgCurrent=0.0, dynPower=0.0;

   	/* First Inverter */

   	Ceq = gatecap(Wcompinvn2+Wcompinvp2,10.0) +
   	      draincap(Wcompinvp1,PCH,1) + draincap(Wcompinvn1,NCH,1);
   	Req = transreson(Wcompinvp1,PCH,1);
   	tf = Req*Ceq;
   	st1del = horowitz(inputtime,tf,VTHCOMPINV,VTHCOMPINV,FALL);
   	nextinputtime = st1del/VTHCOMPINV;
   	dynPower+=Ceq*VddPow*VddPow*2*A;

	if (dualVt == 1)
		lkgCurrent = 0.5 * A * (
			nmos_ileakage(Wcompinvn1/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wcompinvp1/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent = 0.5 * A * (
			nmos_ileakage(Wcompinvn1/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wcompinvp1/Leff,VddPow,Vthp,Tkelvin,Tox) );
   	/* Second Inverter */

   	Ceq = gatecap(Wcompinvn3+Wcompinvp3,10.0) +
   	      draincap(Wcompinvp2,PCH,1) + draincap(Wcompinvn2,NCH,1);
   	Req = transreson(Wcompinvn2,NCH,1);
   	tf = Req*Ceq;
   	st2del = horowitz(nextinputtime,tf,VTHCOMPINV,VTHCOMPINV,RISE);
   	nextinputtime = st2del/(1.0-VTHCOMPINV);
   	dynPower+=Ceq*VddPow*VddPow*2*A;

	if (dualVt == 1)
		lkgCurrent += 0.5 * A * (
			nmos_ileakage(Wcompinvn2/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wcompinvp2/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += 0.5 * A * (
			nmos_ileakage(Wcompinvn2/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wcompinvp2/Leff,VddPow,Vthp,Tkelvin,Tox) );


   	/* Third Inverter */

   	Ceq = gatecap(Wevalinvn+Wevalinvp,10.0) +
   	      draincap(Wcompinvp3,PCH,1) + draincap(Wcompinvn3,NCH,1);
   	Req = transreson(Wcompinvp3,PCH,1);
   	tf = Req*Ceq;
   	st3del = horowitz(nextinputtime,tf,VTHCOMPINV,VTHEVALINV,FALL);
   	nextinputtime = st3del/(VTHEVALINV);
   	dynPower+=Ceq*VddPow*VddPow*2*A;

	if (dualVt == 1)
		lkgCurrent += 0.5 * A * (
			nmos_ileakage(Wcompinvn3/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wcompinvp3/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += 0.5 * A * (
			nmos_ileakage(Wcompinvn3/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wcompinvp3/Leff,VddPow,Vthp,Tkelvin,Tox) );


   	/* Final Inverter (virtual ground driver) discharging compare part */

   	tagbits = ADDRESS_BITS+2 - (int)logtwo((double)C) + (int)logtwo((double)A)-(int)(logtwo(NSubbanks));
   	tagbits=tagbits/2;
   	cols = tagbits*Ntbl*Ntspd;

   	r1 = transreson(Wcompn,NCH,2);
   	r2 = transreson(Wevalinvn,NCH,1); /* was switch */
   	c2 = (tagbits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))+
   	      draincap(Wevalinvp,PCH,1) + draincap(Wevalinvn,NCH,1);
   	c1 = (tagbits)*(draincap(Wcompn,NCH,1)+draincap(Wcompn,NCH,2))
   	     +draincap(Wcompp,PCH,1) + gatecap(WmuxdrvNANDn+WmuxdrvNANDp,20.0) +
   	     cols*Cwordmetal;
   	dynPower+=c2*VddPow*VddPow*2*A;
   	dynPower+=c1*VddPow*VddPow*(A-1);

	if (dualVt == 1)
	{
		lkgCurrent += 0.5 * A * (
			nmos_ileakage(Wevalinvn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wevalinvp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

		// double the tag_bits as the delay is being computed for only
		// half the number. The leakage is still for the total tag bits

		// the comparator compares cols number of bits. Since these transistors are
		// stacked, a stacking factor of 0.2 is used
		lkgCurrent += 2 * cols * 0.2 * 0.5 * A * (
			nmos_ileakage(Wcompn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wcompp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

		// leakage due to the mux driver
		lkgCurrent += 0.5 * A * (
			nmos_ileakage(Wmuxdrv12n/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wmuxdrv12p/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	}
	else
	{
		lkgCurrent += 0.5 * A * (
			nmos_ileakage(Wevalinvn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wevalinvp/Leff,VddPow,Vthp,Tkelvin,Tox) );

		// double the tag_bits as the delay is being computed for only
		// half the number. The leakage is still for the total tag bits

		// the comparator compares cols number of bits. Since these transistors are
		// stacked, a stacking factor of 0.2 is used
		lkgCurrent += 2 * cols * 0.2 * 0.5 * A * (
			nmos_ileakage(Wcompn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wcompp/Leff,VddPow,Vthp,Tkelvin,Tox) );

		// leakage due to the mux driver
		lkgCurrent += 0.5 * A * (
			nmos_ileakage(Wmuxdrv12n/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wmuxdrv12p/Leff,VddPow,Vthp,Tkelvin,Tox) );
	}
   	/* time to go to threshold of mux driver */

   	tstep = (r2*c2+(r1+r2)*c1)*log(1.0/VTHMUXNAND);

   	/* take into account non-zero input rise time */

   	m = Vdd/nextinputtime;

   	if ((tstep) <= (0.5*(Vdd-Vt)/m)) {
  		a = m;
	    b = 2*((Vdd*VTHEVALINV)-Vt);
        c = -2*(tstep)*(Vdd-Vt)+1/m*((Vdd*VTHEVALINV)-Vt)*((Vdd*VTHEVALINV)-Vt);
 	    Tcomparatorni = (-b+sqrt(b*b-4*a*c))/(2*a);
   	} else {
		Tcomparatorni = (tstep) + (Vdd+Vt)/(2*m) - (Vdd*VTHEVALINV)/m;
   	}
   	*outputtime = Tcomparatorni/(1.0-VTHMUXNAND);

	power->readOp.dynamic = dynPower*FREQ;
	power->readOp.leakage = lkgCurrent * VddPow;

	power->writeOp.dynamic = 0.0;
	power->writeOp.leakage = lkgCurrent * VddPow;

   	return(Tcomparatorni+st1del+st2del+st3del);
}




/*----------------------------------------------------------------------*/

/* Delay of the multiplexor Driver (see section 6.7)


double compute_mux_driver_data(int C,int B,int A,int Ndbl,int Nspd,int Ndwl,int Ntbl,int Ntspd,double inputtime,double *outputtime,double wirelength)
{
   	double Ceq,Req,tf,nextinputtime, Cload;
   	double Tst1,Tst2,Tst3;
   	double lkgCurrent;

	// load seen by third driver stage (inverting select to produce select_bar)

	Ceq = BITOUT*gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0) +
   	      	Cwordmetal*wirelength;
   	Cload = Ceq / gatecap(1.0,0.0);
   	Wmuxdrv3p = Cload * SizingRatio * 2/3;
   	Wmuxdrv3n = Cload * SizingRatio /3;

   	WmuxdrvNORn = (Wmuxdrv3n + Wmuxdrv3p) * SizingRatio /3;
   	// 2 to account for series pmos in NOR
   	WmuxdrvNORn = 2* (Wmuxdrv3n + Wmuxdrv3p) * SizingRatio * 2/3;

   	//Wmuxdrv12n = (WmuxdrvNORn+WmuxdrvNORp)*muxover*SizingRatio*1/3;
   	//Wmuxdrv12p = (WmuxdrvNORn+WmuxdrvNORp)*muxover*SizingRatio*2/3;

   	//* first driver stage - Invert "match" to produce "matchb"
   	//* the critical path is the DESELECTED case, so consider what
   	//   happens when the address bit is true, but match goes low

   	Ceq = gatecap(WmuxdrvNORn+WmuxdrvNORp,15.0)*muxover +
   	      draincap(Wmuxdrv12n,NCH,1) + draincap(Wmuxdrv12p,PCH,1);
   	Req = transreson(Wmuxdrv12p,PCH,1);
   	tf = Ceq*Req;
   	Tst1 = horowitz(inputtime,tf,VTHMUXDRV1,VTHMUXDRV2,FALL);
   	nextinputtime = Tst1/VTHMUXDRV2;

	// 0.2 for the stacking of transistors in NOR gate
	if (dualVt == 1)
		lkgCurrent = 0.5 * 0.2 * muxover *(
			nmos_ileakage(WmuxdrvNORn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(WmuxdrvNORp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent = 0.5 * 0.2 * muxover *(
			nmos_ileakage(WmuxdrvNORn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(WmuxdrvNORp/Leff,VddPow,Vthp,Tkelvin,Tox) );


   	//* second driver stage - NOR "matchb" with address bits to produce sel

   	Ceq = gatecap(Wmuxdrv3n+Wmuxdrv3p,15.0) + 2*draincap(WmuxdrvNORn,NCH,1) +
   	      draincap(WmuxdrvNORp,PCH,2);
   	Req = transreson(WmuxdrvNORn,NCH,1);
   	tf = Ceq*Req;
   	Tst2 = horowitz(nextinputtime,tf,VTHMUXDRV2,VTHMUXDRV3,RISE);
   	nextinputtime = Tst2/(1-VTHMUXDRV3);

	if (dualVt == 1)
		lkgCurrent += 0.5 * muxover * (
			nmos_ileakage(Wmuxdrv3n/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wmuxdrv3p/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += 0.5 * muxover * (
			nmos_ileakage(Wmuxdrv3n/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wmuxdrv3p/Leff,VddPow,Vthp,Tkelvin,Tox) );

   	//* third driver stage - invert "select" to produce "select bar"

   	Ceq = BITOUT*gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0)+
   	      draincap(Wmuxdrv3p,PCH,1) + draincap(Wmuxdrv3n,NCH,1) +
   	      Cwordmetal*wirelength;
   	Req = (Rwordmetal*wirelength)/2.0 + transreson(Wmuxdrv3p,PCH,1);
   	tf = Ceq*Req;
   	Tst3 = horowitz(nextinputtime,tf,VTHMUXDRV3,VTHOUTDRINV,FALL);
   	*outputtime = Tst3/(VTHOUTDRINV);

   	return(Tst1 + Tst2 + Tst3);

} */

double compute_mux_driver_dualin_data(int C,int B,int A,int Ndbl,int Nspd,int Ndwl,int Ntbl,int Ntspd,double inputtime1,double *outputtime,double wirelength_v, double wirelength_h, powerDef *power)
{
   	double Ceq,Req,tf,nextinputtime, Cload;
   	double Tst1,Tst2,Tst3;
   	double lkgCurrent=0.0, dynPower = 0.0;

	// load seen by third driver stage (inverting select to produce select_bar)

	Ceq = BITOUT*gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0) +
   	      	Cwordmetal*wirelength_h+Cbitmetal*wirelength_v;
   	Cload = Ceq / gatecap(1.0,0.0);
   	Wmuxdrv3p = Cload * SizingRatio * 2/3;
   	Wmuxdrv3n = Cload * SizingRatio /3;

   	WmuxdrvNORn = (Wmuxdrv3n + Wmuxdrv3p) * SizingRatio /3;
   	// 2 to account for series pmos in NOR
   	WmuxdrvNORn = 2* (Wmuxdrv3n + Wmuxdrv3p) * SizingRatio * 2/3;

   	WmuxdrvNANDn = 2*(WmuxdrvNORn+WmuxdrvNORp)*muxover*SizingRatio*1/3;
   	WmuxdrvNANDp = (WmuxdrvNORn+WmuxdrvNORp)*muxover*SizingRatio*2/3;

   	/* first driver stage - Inverte "match" to produce "matchb" */
   	/* the critical path is the DESELECTED case, so consider what
   	   happens when the address bit is true, but match goes low */

   	Ceq = gatecap(WmuxdrvNORn+WmuxdrvNORp,15.0)*muxover
   	  +draincap(WmuxdrvNANDn,NCH,2) + 2*draincap(WmuxdrvNANDp,PCH,1);
   	Req = transreson(WmuxdrvNANDp,PCH,1);
   	tf = Ceq*Req;
   	Tst1 = horowitz(inputtime1,tf,VTHMUXNAND,VTHMUXDRV2,FALL);
   	nextinputtime = Tst1/VTHMUXDRV2;
   	dynPower+=Ceq*VddPow*VddPow*(A-1);

	if (dualVt == 1) {
		lkgCurrent = 0.5 * 0.2 * muxover * (
			nmos_ileakage(WmuxdrvNORn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(WmuxdrvNORp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

		lkgCurrent = 0.5 * 0.2 * muxover * (
			nmos_ileakage(WmuxdrvNANDn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(WmuxdrvNANDp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

	} else {
		lkgCurrent = 0.5 * 0.2 * muxover * (
			nmos_ileakage(WmuxdrvNORn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(WmuxdrvNORp/Leff,VddPow,Vthp,Tkelvin,Tox) );

		lkgCurrent = 0.5 * 0.2 * muxover * (
			nmos_ileakage(WmuxdrvNANDn/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(WmuxdrvNANDp/Leff,VddPow,Vthp,Tkelvin,Tox) );
	}

   	/* second driver stage - NOR "matchb" with address bits to produce sel */

   	Ceq = gatecap(Wmuxdrv3n+Wmuxdrv3p,15.0) + 2*draincap(WmuxdrvNORn,NCH,1) +
   	      draincap(WmuxdrvNORp,PCH,2);
   	Req = transreson(WmuxdrvNORn,NCH,1);
   	tf = Ceq*Req;
   	Tst2 = horowitz(nextinputtime,tf,VTHMUXDRV2,VTHMUXDRV3,RISE);
   	nextinputtime = Tst2/(1-VTHMUXDRV3);
   	dynPower+=Ceq*VddPow*VddPow;

	if (dualVt == 1)
		lkgCurrent += 0.5 *  muxover * (
			nmos_ileakage(Wmuxdrv3n/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wmuxdrv3p/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += 0.5 *  muxover * (
			nmos_ileakage(Wmuxdrv3n/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wmuxdrv3p/Leff,VddPow,Vthp,Tkelvin,Tox) );

   	/* third driver stage - invert "select" to produce "select bar" */

   	/*   fprintf(stderr, "%f %f %f %f\n",
		   ((Rwordmetal*wirelength)/2.0 + transreson(Wmuxdrv3p,PCH,1)),
		   ((Rwordmetal*0)/2.0 + transreson(Wmuxdrv3p,PCH,1)),
		   (BITOUT*gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0)+
		    draincap(Wmuxdrv3p,PCH,1) + draincap(Wmuxdrv3n,NCH,1) +
		    Cwordmetal*wirelength),
		   (BITOUT*gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0)+
		    draincap(Wmuxdrv3p,PCH,1) + draincap(Wmuxdrv3n,NCH,1) +
		    Cwordmetal*0));
   	*/

   	Ceq = BITOUT*gatecap(Woutdrvseln+Woutdrvselp+Woutdrvnorn+Woutdrvnorp,20.0)+
   	      draincap(Wmuxdrv3p,PCH,1) + draincap(Wmuxdrv3n,NCH,1) +
   	      Cwordmetal*wirelength_h+Cbitmetal*wirelength_v;
   	Req = (Rwordmetal*wirelength_h+Rbitmetal*wirelength_v)/2.0 + transreson(Wmuxdrv3p,PCH,1);
   	tf = Ceq*Req;
   	Tst3 = horowitz(nextinputtime,tf,VTHMUXDRV3,VTHOUTDRINV,FALL);
   	*outputtime = Tst3/(VTHOUTDRINV);
   	dynPower+=Ceq*VddPow*VddPow;

	power->readOp.dynamic = dynPower*FREQ;
	power->readOp.leakage = lkgCurrent * VddPow;

	power->writeOp.dynamic = 0.0;
	power->writeOp.leakage = lkgCurrent * VddPow;


   	return(Tst1 + Tst2 + Tst3);
}

double compute_total_dataout_data(int C,int B,int A,char fullyassoc,int Ndbl,int Nspd,int Ndwl,int Ntbl,int Ntspd,
				double inputtime,double *outputtime, double wirelength_sense_v,double wirelength_sense_h, powerDef *power)
{
   	double Ceq,Req,tf,nextinputtime;
   	double Tst1,Tst2,Tst3;
	double lkgCurrent = 0.0, dynPower=0.0;

   	if(fullyassoc) {
   	     A=1;
   	}

   	/* first driver stage */

   	Ceq = draincap(Wsenseextdrv1p,PCH,1) + draincap(Wsenseextdrv1n,NCH,1) +
   		  gatecap(Wsenseextdrv2n+Wsenseextdrv2p,10.0);
   	Req = transreson(Wsenseextdrv1n,NCH,1);
   	tf = Ceq*Req;
   	Tst1 = horowitz(inputtime,tf,VTHSENSEEXTDRV,VTHSENSEEXTDRV,FALL);
   	nextinputtime = Tst1/VTHSENSEEXTDRV;
   	dynPower+=Ceq*VddPow*VddPow*.5* BITOUT * A * muxover;

	if (dualVt == 1)
		lkgCurrent = 0.5 * BITOUT * A * muxover *(
			nmos_ileakage(Wsenseextdrv1n/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wsenseextdrv1p/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent = 0.5 * BITOUT * A * muxover  *(
			nmos_ileakage(Wsenseextdrv1n/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wsenseextdrv1p/Leff,VddPow,Vthp,Tkelvin,Tox) );
   	/* second driver stage */

   	Ceq = draincap(Wsenseextdrv2p,PCH,1) + draincap(Wsenseextdrv2n,NCH,1) +
   	      Cwordmetal*wirelength_sense_h + Cbitmetal*wirelength_sense_v;

   	Req = (Rwordmetal*wirelength_sense_h + Rbitmetal*wirelength_sense_v)/2.0 + transreson(Wsenseextdrv2p,PCH,1);

   	tf = Ceq*Req;
   	Tst2 = horowitz(nextinputtime,tf,VTHSENSEEXTDRV,VTHOUTDRNAND,RISE);

   	*outputtime = Tst2/(1-VTHOUTDRNAND);
   	dynPower+=Ceq*VddPow*VddPow*.5* BITOUT * A * muxover;

	if (dualVt == 1)
		lkgCurrent = 0.5 * BITOUT * A * muxover  *(
			nmos_ileakage(Wsenseextdrv2n/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wsenseextdrv2p/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent = 0.5 * BITOUT * A * muxover  *(
			nmos_ileakage(Wsenseextdrv2n/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wsenseextdrv2p/Leff,VddPow,Vthp,Tkelvin,Tox) );

   	//   fprintf(stderr, "Pow %f %f\n", Ceq*VddPow*VddPow*.5*BITOUT*A*muxover*1e3, Ceq*VddPow*VddPow*.5*BITOUT*A*muxover*1e3);

	power->readOp.dynamic = dynPower*FREQ;
	power->readOp.leakage = lkgCurrent * VddPow;

	power->writeOp.dynamic = 0.0;
	power->writeOp.leakage = lkgCurrent * VddPow;

   	return(Tst1 + Tst2);

}

/*
double compute_total_out_driver_data(int C,int B,int A,char fullyassoc,int Ndbl,int Nspd,int Ndwl,int Ntbl,int Ntwl,int Ntspd,
				double NSubbanks,double inputtime,double *outputtime,powerDef *power)
{
	void reset_powerDef(powerDef *power);
	void add_powerDef_vals(powerDef *sum, powerDef src1, powerDef src2);

	double total_senseext_driver,single_senseext_driver;
	powerDef single_power;
	int cols_data_subarray,rows_data_subarray,cols_tag_subarray,rows_tag_subarray;
	double subbank_v,subbank_h, sub_v,sub_h,inter_v,inter_h,htree;
	int htree_int,tagbits;
	int cols_fa_subarray,rows_fa_subarray;
	double lkgCurrent=0.0, dynPower=0.0;

	reset_powerDef(&single_power);
	total_senseext_driver=0.0;
	single_senseext_driver=0.0;

	if(!fullyassoc) {
    	cols_data_subarray = (8*B*A*Nspd/Ndwl);
        rows_data_subarray = (C/(B*A*Ndbl*Nspd));

        if(Ndwl*Ndbl==1) {
    		sub_v= rows_data_subarray;
            sub_h= cols_data_subarray;
        }
        if(Ndwl*Ndbl==2) {
            sub_v= rows_data_subarray;
            sub_h= 2*cols_data_subarray;
        }
        if(Ndwl*Ndbl>2) {
            htree=logtwo((double)(Ndwl*Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
    	        sub_v = sqrt(Ndwl*Ndbl)*rows_data_subarray;
                sub_h = sqrt(Ndwl*Ndbl)*cols_data_subarray;
            }
            else {
                sub_v = sqrt(Ndwl*Ndbl/2)*rows_data_subarray;
                sub_h = 2*sqrt(Ndwl*Ndbl/2)*cols_data_subarray;
            }
        }

        inter_v=sub_v;
        inter_h=sub_h;

        rows_tag_subarray = C/(B*A*Ntbl*Ntspd);

        tagbits = ADDRESS_BITS+2-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));
        cols_tag_subarray = tagbits*A*Ntspd/Ntwl ;

        if(Ntwl*Ntbl==1) {
        	sub_v= rows_tag_subarray;
            sub_h= cols_tag_subarray;
        }
        if(Ntwl*Ntbl==2) {
            sub_v= rows_tag_subarray;
            sub_h= 2*cols_tag_subarray;
        }

        if(Ntwl*Ntbl>2) {
            htree=logtwo((double)(Ndwl*Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
        	    sub_v = sqrt(Ndwl*Ndbl)*rows_tag_subarray;
                sub_h = sqrt(Ndwl*Ndbl)*cols_tag_subarray;
            }
            else {
                sub_v = sqrt(Ndwl*Ndbl/2)*rows_tag_subarray;
                sub_h = 2*sqrt(Ndwl*Ndbl/2)*cols_tag_subarray;
            }
        }

        inter_v=MAX(sub_v,inter_v);
        inter_h+=sub_h;
        subbank_h = inter_h;
        subbank_v = inter_v;
  	}
  	else {
  		rows_fa_subarray = (C/(B*Ndbl));
        tagbits = ADDRESS_BITS+2-(int)logtwo((double)B);
        cols_tag_subarray = tagbits ;
        cols_fa_subarray = (8*B)+cols_tag_subarray;

        if(Ndbl==1) {
  	    	sub_v= rows_fa_subarray;
        	sub_h= cols_fa_subarray;
        }
        if(Ndbl==2) {
            sub_v= rows_fa_subarray;
            sub_h= 2*cols_fa_subarray;
        }

        if(Ndbl>2) {
            htree=logtwo((double)(Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
        	    sub_v = sqrt(Ndbl)*rows_fa_subarray;
                sub_h = sqrt(Ndbl)*cols_fa_subarray;
            }
            else {
                sub_v = sqrt(Ndbl/2)*rows_fa_subarray;
                sub_h = 2*sqrt(Ndbl/2)*cols_fa_subarray;
            }
		}
    	inter_v=sub_v;
    	inter_h=sub_h;

    	subbank_v=inter_v;
    	subbank_h=inter_h;
	}

    if(NSubbanks==1.0 || NSubbanks==2.0 ) {
	    subbank_h = 0;
	    subbank_v = 0;
	    single_senseext_driver=compute_total_dataout_data(C,B,A,fullyassoc,Ndbl,Nspd,Ndwl,Ntbl,Ntspd,(BITOUT*A*muxover),inputtime,outputtime, subbank_v, subbank_h, &single_power);
		single_power.writeOp.dynamic = 0.0;
        total_senseext_driver+=single_senseext_driver;
        add_powerDef_vals(power,(*power),single_power);
	}

    while(NSubbanks > 2.0) {
		if(NSubbanks==4.0) {
			subbank_h = 0;
		}

	  	single_senseext_driver=compute_total_dataout_data(C,B,A,fullyassoc,Ndbl,Nspd,Ndwl,Ntbl,Ntspd,(BITOUT*A*muxover),inputtime,outputtime, subbank_v, subbank_h, &single_power);
	  	single_power.writeOp.dynamic = 0.0;

		NSubbanks=NSubbanks/4.0;
		subbank_v=2*subbank_v;
		subbank_h=2*subbank_h;
        inputtime = *outputtime;
		total_senseext_driver+=single_senseext_driver;
		add_powerDef_vals(power,(*power),single_power);
	}

	return(total_senseext_driver);
}
 */
/* Valid driver (see section 6.9 of tech report)
   Note that this will only be called for a direct mapped cache */

double compute_valid_driver_data(int C,int B,int A,char fullyassoc,int Ndbl,int Ndwl,int Nspd,int Ntbl,int Ntwl,int Ntspd,double NSubbanks,double inputtime,powerDef *power)
{
   	double Ceq,Tst1,tf;
   	int rows,tagbits,cols,htree_int,l_valdrv_v,l_valdrv_h,exp;
   	double wire_cap,wire_res;
   	double htree;
   	double subbank_v,subbank_h;

   	rows = C/(8*B*A*Ntbl*Ntspd);

   	tagbits = ADDRESS_BITS+2-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));
   	cols = tagbits*A*Ntspd/Ntwl ;

   	/* calculate some layout info */

   	if(Ntwl*Ntbl==1) {
		l_valdrv_v= 0;
   	    l_valdrv_h= cols;
   	}
   	if(Ntwl*Ntbl==2 || Ntwl*Ntbl==4) {
        l_valdrv_v= 0;
        l_valdrv_h= 2*cols;
   	}
	if(Ntwl*Ntbl>4) {
   	    htree=logtwo((double)(Ntwl*Ntbl));
   	    htree_int = (int) htree;
   	    if (htree_int % 2 ==0) {
  		   	exp = (htree_int/2-1);
   	      	l_valdrv_v = (powers(2,exp)-1)*rows;
          	l_valdrv_h = sqrt(Ntwl*Ntbl)*cols;
        }
        else {
            exp = (htree_int+1)/2-1;
            l_valdrv_v = (powers(2,exp)-1)*rows;
            l_valdrv_h = sqrt(Ntwl*Ntbl/2)*cols;
        }
    }

   	subbank_routing_length(C,B,A,fullyassoc,Ndbl,Nspd,Ndwl,Ntbl,Ntwl,Ntspd,NSubbanks,&subbank_v,&subbank_h);

   	wire_cap = Cbitmetal*(l_valdrv_v+subbank_v) +Cwordmetal*(l_valdrv_h+subbank_h);
   	wire_res = Rwordmetal*(l_valdrv_h+subbank_h)*0.5+Rbitmetal*(l_valdrv_v+subbank_v)*0.5;

   	Ceq = draincap(Wmuxdrv12n,NCH,1)+draincap(Wmuxdrv12p,PCH,1)+wire_cap+Cout;
   	tf = Ceq*(transreson(Wmuxdrv12p,PCH,1)+wire_res);
   	Tst1 = horowitz(inputtime,tf,VTHMUXDRV1,0.5,FALL);

   	power->readOp.dynamic += Ceq*VddPow*VddPow*FREQ;

   	return(Tst1);
}


double compute_datain_power(int C,int B,int A,char fullyassoc,int Ndbl,int Nspd,int Ndwl, powerDef *power)
{
	double Ceq,Rwire,Rline;
    double aspectRatio;     /* as height over width */
    double ramBlocks;       /* number of RAM blocks */
    double tf;
    double nordel,outdel,nextinputtime;
    double hstack,vstack;
	double l_outdrv_v,l_outdrv_h;
	double rows,cols,rows_fa_subarray,cols_fa_subarray;
	double htree, Cload;
	int htree_int,exp,tagbits;
	double lkgCurrent = 0.0, dynPower=0.0;

	if(!fullyassoc) {

		rows = (C/(B*A*Ndbl*Nspd));
		cols = (8*B*A*Nspd/Ndwl);

    	// calculate some layout info

        if(Ndwl*Ndbl==1) {
        	l_outdrv_v= 0;
            l_outdrv_h= cols;
        }
        if(Ndwl*Ndbl==2 || Ndwl*Ndbl==4) {
            l_outdrv_v= 0;
            l_outdrv_h= 2*cols;
        }
        if(Ndwl*Ndbl>4) {
            htree=logtwo((double)(Ndwl*Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
		  		exp = (htree_int/2-1);
                l_outdrv_v = (powers(2,exp)-1)*rows;
                l_outdrv_h = sqrt(Ndwl*Ndbl)*cols;
            }
            else {
		  		exp = (htree_int+1)/2-1;
                l_outdrv_v = (powers(2,exp)-1)*rows;
                l_outdrv_h = sqrt(Ndwl*Ndbl/2)*cols;
            }
        }
  	}
  	else {
  		rows_fa_subarray = (C/(B*Ndbl));
    	tagbits = ADDRESS_BITS+2-(int)logtwo((double)B);
        cols_fa_subarray = (8*B)+tagbits;
        if(Ndbl==1) {
    	    l_outdrv_v= 0;
            l_outdrv_h= cols_fa_subarray;
        }
        if(Ndbl==2 || Ndbl==4) {
            l_outdrv_v= 0;
            l_outdrv_h= 2*cols_fa_subarray;
        }
        if(Ndbl>4) {
            htree=logtwo((double)(Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
            	exp = (htree_int/2-1);
                l_outdrv_v = (powers(2,exp)-1)*rows_fa_subarray;
                l_outdrv_h = sqrt(Ndbl)*cols_fa_subarray;
            }
            else {
                exp = (htree_int+1)/2-1;
                l_outdrv_v = (powers(2,exp)-1)*rows_fa_subarray;
                l_outdrv_h = sqrt(Ndbl/2)*cols_fa_subarray;
            }
        }
	}

    /* first driver stage */
    // the driver width is assumed to be same as the sense ext driver size
	Ceq = draincap(Wsenseextdrv1p,PCH,1) + draincap(Wsenseextdrv1n,NCH,1) +
	   		  gatecap(Wsenseextdrv2n+Wsenseextdrv2p,10.0);

	// there should be atleast 8*B drivers (to enable cacheline write) -- used in lkg power
	// but during normal write only BITIN drivers might switch. -- used in dynamic power
	dynPower+=Ceq*VddPow*VddPow*.5* BITIN;

	if (dualVt == 1)
		lkgCurrent = 0.5 * 8*B * (
			nmos_ileakage(Wsenseextdrv1n/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wsenseextdrv1p/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent = 0.5 * 8*B *(
			nmos_ileakage(Wsenseextdrv1n/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wsenseextdrv1p/Leff,VddPow,Vthp,Tkelvin,Tox) );

	/* second driver stage */
	Ceq = draincap(Wsenseextdrv2p,PCH,1) + draincap(Wsenseextdrv2n,NCH,1) +
	  	      Cwordmetal*l_outdrv_h + Cbitmetal*l_outdrv_v ;

	// there should be atleast 8*B drivers (to enable cacheline write) -- used in leakage power
	// but during normal write only BITIN drivers might switch. -- used in dynamic power
   	dynPower+=Ceq*VddPow*VddPow*.5* BITIN ;

	if (dualVt == 1)
		lkgCurrent = 0.5 * 8*B * (
			nmos_ileakage(Wsenseextdrv2n/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Wsenseextdrv2p/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent = 0.5 * 8*B *(
			nmos_ileakage(Wsenseextdrv2n/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Wsenseextdrv2p/Leff,VddPow,Vthp,Tkelvin,Tox) );

   	//   fprintf(stderr, "Pow %f %f\n", Ceq*VddPow*VddPow*.5*BITOUT*A*muxover*1e3, Ceq*VddPow*VddPow*.5*BITOUT*A*muxover*1e3);

	power->readOp.dynamic = 0.0;
	power->readOp.leakage = lkgCurrent * VddPow;

	power->writeOp.dynamic = dynPower*FREQ;
	power->writeOp.leakage = lkgCurrent * VddPow;
}

/*----------------------------------------------------------------------*/

/* Data output delay (data side) -- see section 6.8
   This is the time through the NAND/NOR gate and the final inverter
   assuming sel is already present */

double compute_dataoutput_data(int C,int B,int A,char fullyassoc,int Ndbl,int Nspd,int Ndwl,
              double inrisetime,double *outrisetime,powerDef *power)
{
	double Ceq,Rwire,Rline;
    double aspectRatio;     /* as height over width */
    double ramBlocks;       /* number of RAM blocks */
    double tf;
    double nordel,outdel,nextinputtime;
    double hstack,vstack;
	double l_outdrv_v,l_outdrv_h;
	double rows,cols,rows_fa_subarray,cols_fa_subarray;
	double htree, Cload;
	int htree_int,exp,tagbits;
	double lkgCurrent = 0.0, dynPower=0.0;

	if(!fullyassoc) {

		rows = (C/(B*A*Ndbl*Nspd));
		cols = (8*B*A*Nspd/Ndwl);

    	// calculate some layout info

        if(Ndwl*Ndbl==1) {
        	l_outdrv_v= 0;
            l_outdrv_h= cols;
        }
        if(Ndwl*Ndbl==2 || Ndwl*Ndbl==4) {
            l_outdrv_v= 0;
            l_outdrv_h= 2*cols;
        }
        if(Ndwl*Ndbl>4) {
            htree=logtwo((double)(Ndwl*Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
		  		exp = (htree_int/2-1);
                l_outdrv_v = (powers(2,exp)-1)*rows;
                l_outdrv_h = sqrt(Ndwl*Ndbl)*cols;
            }
            else {
		  		exp = (htree_int+1)/2-1;
                l_outdrv_v = (powers(2,exp)-1)*rows;
                l_outdrv_h = sqrt(Ndwl*Ndbl/2)*cols;
            }
        }
  	}
  	else {
  		rows_fa_subarray = (C/(B*Ndbl));
    	tagbits = ADDRESS_BITS+2-(int)logtwo((double)B);
        cols_fa_subarray = (8*B)+tagbits;
        if(Ndbl==1) {
    	    l_outdrv_v= 0;
            l_outdrv_h= cols_fa_subarray;
        }
        if(Ndbl==2 || Ndbl==4) {
            l_outdrv_v= 0;
            l_outdrv_h= 2*cols_fa_subarray;
        }
        if(Ndbl>4) {
            htree=logtwo((double)(Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
            	exp = (htree_int/2-1);
                l_outdrv_v = (powers(2,exp)-1)*rows_fa_subarray;
                l_outdrv_h = sqrt(Ndbl)*cols_fa_subarray;
            }
            else {
                exp = (htree_int+1)/2-1;
                l_outdrv_v = (powers(2,exp)-1)*rows_fa_subarray;
                l_outdrv_h = sqrt(Ndbl/2)*cols_fa_subarray;
            }
        }
	}
	// typical width of gate considered for the estimating val of draincap.
	/* Cload = (draincap(5.0,NCH,1)+draincap(5.0,PCH,1))*A*muxover +
						Cwordmetal*l_outdrv_h + Cbitmetal*l_outdrv_v +
						gatecap(Wsenseextdrv1n+Wsenseextdrv1p,10.0);

	Woutdrivern = 	(Cload/gatecap(1.0,0.0))*SizingRatio/3;
	Woutdriverp = 	(Cload/gatecap(1.0,0.0))*SizingRatio*2/3;

	// eff load for nor gate = gatecap(drv_p);
	// factor of 2 is needed to account for series nmos transistors in nor2
	Woutdrvnorp = 2*Woutdriverp*SizingRatio*2/3;
	Woutdrvnorn = Woutdriverp*SizingRatio/3;

	// factor of 2 is needed to account for series nmos transistors in nand2
	Woutdrvnandp = Woutdrivern*SizingRatio*2/3;
	Woutdrvnandn = 2*Woutdrivern*SizingRatio/3;

	Woutdrvselp = (Woutdrvnandp + Woutdrvnandn) * SizingRatio*2/3;
	Woutdrvseln = (Woutdrvnandp + Woutdrvnandn) * SizingRatio/3;

	*/
    /* Delay of NOR gate */

    Ceq = 2*draincap(Woutdrvnorn,NCH,1)+draincap(Woutdrvnorp,PCH,2)+
          gatecap(Woutdrivern,10.0);

    tf = Ceq*transreson(Woutdrvnorp,PCH,2);
    nordel = horowitz(inrisetime,tf,VTHOUTDRNOR,VTHOUTDRIVE,FALL);
    nextinputtime = nordel/(VTHOUTDRIVE);
	dynPower+=Ceq*VddPow*VddPow*.5*BITOUT;

	if (dualVt == 1) {
		lkgCurrent += 0.5 * BITOUT * A * muxover *(
				nmos_ileakage(Woutdrvseln/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
				pmos_ileakage(Woutdrvselp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

		lkgCurrent += 0.5 * 0.2 * BITOUT * A * muxover *(
				nmos_ileakage(Woutdrvnorn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
				pmos_ileakage(Woutdrvnorp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );

		lkgCurrent += 0.5 * 0.2 * BITOUT * A * muxover *(
				nmos_ileakage(Woutdrvnandn/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
				pmos_ileakage(Woutdrvnandp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	} else
	{
		lkgCurrent += 0.5 * BITOUT * A * muxover *(
				nmos_ileakage(Woutdrvseln/Leff,VddPow,Vthn,Tkelvin,Tox) +
				pmos_ileakage(Woutdrvselp/Leff,VddPow,Vthp,Tkelvin,Tox) );

		lkgCurrent += 0.5 * 0.2 * BITOUT * A * muxover *(
				nmos_ileakage(Woutdrvnorn/Leff,VddPow,Vthn,Tkelvin,Tox) +
				pmos_ileakage(Woutdrvnorp/Leff,VddPow,Vthp,Tkelvin,Tox) );

		lkgCurrent += 0.5 * 0.2 * BITOUT * A * muxover *(
				nmos_ileakage(Woutdrvnandn/Leff,VddPow,Vthn,Tkelvin,Tox) +
				pmos_ileakage(Woutdrvnandp/Leff,VddPow,Vthp,Tkelvin,Tox) );
	}

    /* Delay of final output driver */

    Ceq = (draincap(Woutdrivern,NCH,1)+draincap(Woutdriverp,PCH,1))*A*muxover
	     +Cwordmetal*l_outdrv_h + Cbitmetal*l_outdrv_v + gatecap(Wsenseextdrv1n+Wsenseextdrv1p,10.0);
    Rwire = (Rwordmetal*l_outdrv_h + Rbitmetal*l_outdrv_v)/2;

	dynPower+=Ceq*VddPow*VddPow*.5*BITOUT;
	if (dualVt == 1)
		lkgCurrent += 0.5 * BITOUT * A * muxover *(
			nmos_ileakage(Woutdrivern/Leff,VddPow,Vt_bit_nmos_low,Tkelvin,Tox) +
			pmos_ileakage(Woutdriverp/Leff,VddPow,Vt_bit_pmos_low,Tkelvin,Tox) );
	else
		lkgCurrent += 0.5 * BITOUT * A * muxover *(
			nmos_ileakage(Woutdrivern/Leff,VddPow,Vthn,Tkelvin,Tox) +
			pmos_ileakage(Woutdriverp/Leff,VddPow,Vthp,Tkelvin,Tox) );


    tf = Ceq*(transreson(Woutdrivern,NCH,1)+Rwire);
    outdel = horowitz(nextinputtime,tf,VTHOUTDRIVE,0.5,RISE);
    *outrisetime = outdel/0.5;

    power->readOp.dynamic = dynPower*FREQ;
    power->readOp.leakage = lkgCurrent * VddPow;

	power->writeOp.dynamic = 0.0;
    power->writeOp.leakage = lkgCurrent * VddPow;

   	return(outdel+nordel);
}

/*----------------------------------------------------------------------*/

/* Sel inverter delay (part of the output driver)  see section 6.8 */

double compute_selb_tag_path_data(double inrisetime,double *outrisetime,powerDef *power)
{
   double Ceq,Tst1,tf;

// leakage is already accounted in mux driver delay function.

   Ceq = draincap(Woutdrvseln,NCH,1)+draincap(Woutdrvselp,PCH,1)+
         gatecap(Woutdrvnandn+Woutdrvnandp,10.0);
   tf = Ceq*transreson(Woutdrvseln,NCH,1);
   Tst1 = horowitz(inrisetime,tf,VTHOUTDRINV,VTHOUTDRNAND,RISE);
   *outrisetime = Tst1/(1.0-VTHOUTDRNAND);
   power->readOp.dynamic +=Ceq*VddPow*VddPow*FREQ;

   return(Tst1);
}


/*----------------------------------------------------------------------*/

/* This routine calculates the extra time required after an access before
 * the next access can occur [ie.  it returns (cycle time-access time)].
 */

double compute_precharge_data(double worddata)
{
   double Ceq,tf,pretime;

   /* as discussed in the tech report, the delay is the delay of
      4 inverter delays (each with fanout of 4) plus the delay of
      the wordline */

   Ceq = draincap(Wdecinvn,NCH,1)+draincap(Wdecinvp,PCH,1)+
         4*gatecap(Wdecinvn+Wdecinvp,0.0);
   tf = Ceq*transreson(Wdecinvn,NCH,1);
   pretime = 4*horowitz(0.0,tf,0.5,0.5,RISE) + worddata;

   return(pretime);
}


void compute_device_widths(int C,int B,int A,int fullyassoc, int Ndwl,int Ndbl,int Nspd)
{
	int rows, cols, numstack,l_predec_nor_v,l_predec_nor_h, htree_int, total_edge_length,exp;
	double desiredrisetime, Rpdrive, Cline, Cload, htree;
	double effWdecNORn,effWdecNORp,effWdec3to8n,effWdec3to8p, wire_res, wire_cap;
	double l_inv_predecode;
	double l_outdrv_v,l_outdrv_h;
	double rows_fa_subarray,cols_fa_subarray, tagbits;

	rows = C/(8*B*A*Ndbl*Nspd);
	cols = 8*B*A*Nspd/Ndwl;

	// Wordline capacitance to determine the wordline driver size
	/* Use a first-order approx */
	desiredrisetime = krise*log((double)(cols))/2.0;
	Cline = (gatecappass(Wmemcella,0.0)+ gatecappass(Wmemcella,0.0)+
			 	Cwordmetal)*cols;
	Rpdrive = desiredrisetime/(Cline*log(VSINV)*-1.0);
	WwlDrvp = restowidth(Rpdrive,PCH);
	if (WwlDrvp > Wworddrivemax) {
		WwlDrvp = Wworddrivemax;
	}

	/* Now that we have a reasonable psize, do the rest as before */
	/* If we keep the ratio the same as the tag wordline driver,
	   the threshold voltage will be close to VSINV */

	/* assuming that nsize is half the nsize */
	WwlDrvn = WwlDrvp/2;

	// Size of wordline
	// Sizing ratio for optimal delay is 3-4.
	Wdecinvn = (WwlDrvp + WwlDrvn) * SizingRatio * 1/3;
	Wdecinvp = (WwlDrvp + WwlDrvn) * SizingRatio * 2/3;

	// determine size of nor and nand gates in the decoder

	// width of NOR driving decInv -
	// effective width (NORp + NORn = Cout/SizingRatio( FANOUT))
	// Cout = Wdecinvn + Wdecinvp; SizingRatio = 3;
	// nsize = effWidth/3; psize = 2*effWidth/3;

	numstack =
	       ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ndbl*Nspd))));
	if (numstack==0) numstack = 1;

	if (numstack>5) numstack = 5;

	effWdecNORn = (Wdecinvn + Wdecinvp)*SizingRatio/3;
	effWdecNORp = 2*(Wdecinvn + Wdecinvp)*SizingRatio/3;
	WdecNORn = effWdecNORn;
	WdecNORp = effWdecNORp * numstack;

	/* second stage: driving a bunch of nor gates with a nand */
	if(Ndwl*Ndbl==1 || Ndwl*Ndbl==2 || Ndwl*Ndbl==4) {
	  	l_predec_nor_v = 0;
	    l_predec_nor_h = 0;
	} else {
		if(Ndwl*Ndbl==8) {
	   	   l_predec_nor_v = 0;
	   	   l_predec_nor_h = cols;
		}
	}

	if(Ndwl*Ndbl>8) {
	    htree=logtwo((double)(Ndwl*Ndbl));
	    htree_int = (int) htree;
	    if (htree_int % 2 ==0) {
	  	    exp = (htree_int/2-1);
	        l_predec_nor_v = (powers(2,exp)-1)*rows*8;
		  	l_predec_nor_h = (powers(2,exp)-1)*cols;
	    } else {
	        exp = (htree_int-3)/2;
	        l_predec_nor_v = (powers(2,exp)-1)*rows*8;
		    l_predec_nor_h = (powers(2,(exp+1))-1)*cols;
	   	}
	}

	Cline = gatecap(WdecNORn+WdecNORp,((numstack*40)+20.0))*rows/8 +
	  			Cbitmetal*(rows*8*l_predec_nor_v)+Cwordmetal*(l_predec_nor_h);

	Cload = Cline / gatecap(1.0,0.0);

	effWdec3to8n = Cload*SizingRatio/3;
	effWdec3to8p = 2*Cload*SizingRatio/3;

	Wdec3to8n = effWdec3to8n * 3; // nand3 gate
	Wdec3to8p = effWdec3to8p;

	// size of address drivers before decoders
	/* First stage: driving the decoders */
	total_edge_length = 8*B*A*Ndbl*Nspd ;
	if(Ndwl*Ndbl==1 ) {
		l_inv_predecode = 1;
	    wire_cap = Cwordmetal*l_inv_predecode*total_edge_length;
	    wire_res = 0.5*Rwordmetal*l_inv_predecode*total_edge_length;
	}

	if(Ndwl*Ndbl==2 ) {
	    l_inv_predecode = 0.5;
	    wire_cap = Cwordmetal*l_inv_predecode*total_edge_length;
	    wire_res = 0.5*Rwordmetal*l_inv_predecode*total_edge_length;
	}

	if(Ndwl*Ndbl>2) {
		htree=logtwo((double)(Ndwl*Ndbl));
		htree_int = (int) htree;
	    if (htree_int % 2 ==0) {
			exp = (htree_int/2-1);
			l_inv_predecode = 0.25/(powers(2,exp));
	        wire_cap = Cwordmetal*l_inv_predecode*total_edge_length;
	        wire_res = 0.5*Rwordmetal*l_inv_predecode*total_edge_length;
	  	}
	   	else {
			exp = (htree_int-3)/2;
			l_inv_predecode = powers(2,exp);
	        wire_cap = Cbitmetal*l_inv_predecode*rows*8;
	        wire_res = 0.5*Rbitmetal*l_inv_predecode*rows*8;
	   	}
	}

	Cline = 4*gatecap(Wdec3to8n+Wdec3to8p,10.0)*(Ndwl*Ndbl)+wire_cap;
	Cload = Cline / gatecap(1.0,0.0);

	Wdecdriven1 = Cload*SizingRatio/3;
	Wdecdrivep1 = 2*Cload*SizingRatio/3;

	// Size of second driver

	Wdecdriven2 = (Wdecdriven1 + Wdecdrivep1)*SizingRatio/3;
	Wdecdrivep2 = 2*(Wdecdriven1 + Wdecdrivep1)*SizingRatio/3;

	// these are the widths of the devices of dataoutput devices
	// will be used in the data_senseamplifier_data and dataoutput_data functions

	if(!fullyassoc) {

		rows = (C/(B*A*Ndbl*Nspd));
		cols = (8*B*A*Nspd/Ndwl);

	    /* calculate some layout info */

		if(Ndwl*Ndbl==1) {
	       	l_outdrv_v= 0;
	        l_outdrv_h= cols;
	    }
	    if(Ndwl*Ndbl==2 || Ndwl*Ndbl==4) {
	        l_outdrv_v= 0;
	        l_outdrv_h= 2*cols;
	    }
	    if(Ndwl*Ndbl>4) {
	        htree=logtwo((double)(Ndwl*Ndbl));
	        htree_int = (int) htree;

	    	if (htree_int % 2 ==0) {
				exp = (htree_int/2-1);
				l_outdrv_v = (powers(2,exp)-1)*rows;
				l_outdrv_h = sqrt(Ndwl*Ndbl)*cols;
	        }
	        else {
			  	exp = (htree_int+1)/2-1;
	            l_outdrv_v = (powers(2,exp)-1)*rows;
	            l_outdrv_h = sqrt(Ndwl*Ndbl/2)*cols;
	        }
	    }
	}
  	else {
  		rows_fa_subarray = (C/(B*Ndbl));
    	tagbits = ADDRESS_BITS+2-(int)logtwo((double)B);
        cols_fa_subarray = (8*B)+tagbits;
        if(Ndbl==1) {
    	    l_outdrv_v= 0;
            l_outdrv_h= cols_fa_subarray;
        }
        if(Ndbl==2 || Ndbl==4) {
            l_outdrv_v= 0;
            l_outdrv_h= 2*cols_fa_subarray;
        }
        if(Ndbl>4) {
            htree=logtwo((double)(Ndbl));
            htree_int = (int) htree;
            if (htree_int % 2 ==0) {
            	exp = (htree_int/2-1);
                l_outdrv_v = (powers(2,exp)-1)*rows_fa_subarray;
                l_outdrv_h = sqrt(Ndbl)*cols_fa_subarray;
            }
            else {
                exp = (htree_int+1)/2-1;
                l_outdrv_v = (powers(2,exp)-1)*rows_fa_subarray;
                l_outdrv_h = sqrt(Ndbl/2)*cols_fa_subarray;
            }
        }
	}
	// typical width of gate considered for the estimating val of draincap.
	Cload = (draincap(5.0,NCH,1)+draincap(5.0,PCH,1))*A*muxover +
						Cwordmetal*l_outdrv_h + Cbitmetal*l_outdrv_v +
						gatecap(Wsenseextdrv1n+Wsenseextdrv1p,10.0);

	Woutdrivern = 	(Cload/gatecap(1.0,0.0))*SizingRatio/3;
	Woutdriverp = 	(Cload/gatecap(1.0,0.0))*SizingRatio*2/3;

	// eff load for nor gate = gatecap(drv_p);
	// factor of 2 is needed to account for series nmos transistors in nor2
	Woutdrvnorp = 2*Woutdriverp*SizingRatio*2/3;
	Woutdrvnorn = Woutdriverp*SizingRatio/3;

	// factor of 2 is needed to account for series nmos transistors in nand2
	Woutdrvnandp = Woutdrivern*SizingRatio*2/3;
	Woutdrvnandn = 2*Woutdrivern*SizingRatio/3;

	Woutdrvselp = (Woutdrvnandp + Woutdrvnandn) * SizingRatio*2/3;
	Woutdrvseln = (Woutdrvnandp + Woutdrvnandn) * SizingRatio/3;

}


void compute_tag_device_widths(int C,int B,int A,int Ndwl,int Ndbl,int Nspd,int Ntspd,int Ntwl,int Ntbl,double NSubbanks)
{

	int rows,cols, tagbits, numstack,l_predec_nor_v,l_predec_nor_h, htree_int, total_edge_length, exp;
	double desiredrisetime, Rpdrive, Cline, Cload, htree;
	double effWtdecNORn,effWtdecNORp,effWtdec3to8n,effWtdec3to8p, wire_res, wire_cap;
	double l_inv_predecode;


	rows = C/(8*B*A*Ntbl*Ntspd);
	tagbits = ADDRESS_BITS+2-(int)logtwo((double)C)+(int)logtwo((double)A)-(int)(logtwo(NSubbanks));
	cols = tagbits * A * Ntspd/Ntwl;

    // capacitive load on the wordline - C_int + C_memCellLoad * NCells
	Cline = (gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+
			 gatecappass(Wmemcella,(BitWidth-2*Wmemcella)/2.0)+ Cwordmetal)*cols;

	Cload = Cline / gatecap(1.0,0.0);

	// apprx width of the driver for optimal delay
	WtwlDrvn = Cload*SizingRatio/3; WtwlDrvp = 2*Cload*SizingRatio/3;

	Wtdecinvn = (WtwlDrvn + WtwlDrvp)*SizingRatio*1/3;
	Wtdecinvp = (WtwlDrvn + WtwlDrvp)*SizingRatio*2/3;

	// determine widths of nand, nor gates in the tag decoder
	// width of NOR driving decInv -
	// effective width (NORp + NORn = Cout/SizingRatio( FANOUT))
	// Cout = Wdecinvn + Wdecinvp; SizingRatio = 3;
	// nsize = effWidth/3; psize = 2*effWidth/3;
	numstack =
	    ceil((1.0/3.0)*logtwo( (double)((double)C/(double)(B*A*Ntbl*Ntspd))));
	if (numstack==0) numstack = 1;
	if (numstack>5) numstack = 5;

	effWtdecNORn = (Wtdecinvn + Wtdecinvp)*SizingRatio/3;
	effWtdecNORp = 2*(Wtdecinvn + Wtdecinvp)*SizingRatio/3;
	WtdecNORn = effWtdecNORn;
	WtdecNORp = effWtdecNORp * numstack;

	if(Ntwl*Ntbl==1 || Ntwl*Ntbl==2 || Ntwl*Ntbl==4) {
		l_predec_nor_v = 0;
	   	l_predec_nor_h = 0;
	} else {
	    if(Ntwl*Ntbl==8) {
			l_predec_nor_v = 0;
			l_predec_nor_h = cols;
        }
    }

	if(Ntwl*Ntbl>8) {
		htree=logtwo((double)(Ntwl*Ntbl));
	    htree_int = (int) htree;
	    if (htree_int % 2 ==0) {
			exp = (htree_int/2-1);
	        l_predec_nor_v = (powers(2,exp)-1)*rows*8;
	        l_predec_nor_h = (powers(2,exp)-1)*cols;
	    }
	    else {
			exp = (htree_int-3)/2;
	        l_predec_nor_v = (powers(2,exp)-1)*rows*8;
	        l_predec_nor_h = (powers(2,(exp+1))-1)*cols;
	    }
	}

	// find width of the nand gates in the 3-8 decoders
	Cline = gatecap(WtdecNORn+WtdecNORp,((numstack*40)+20.0))*rows/8 +
           Cbitmetal*(rows*8*l_predec_nor_v) +Cwordmetal*(l_predec_nor_h);

	Cload = Cline / gatecap(1.0,0.0);

    effWtdec3to8n = Cload*SizingRatio/3;
	effWtdec3to8p = 2*Cload*SizingRatio/3;

	Wtdec3to8n = effWtdec3to8n * 3; // nand3 gate
	Wtdec3to8p = effWtdec3to8p;

	total_edge_length = cols*Ntwl*Ntbl ;

    if(Ntwl*Ntbl==1 ) {
    	l_inv_predecode = 1;
        wire_cap = Cwordmetal*l_inv_predecode*total_edge_length;
        wire_res = 0.5*Rwordmetal*l_inv_predecode*total_edge_length;
    }

    if(Ntwl*Ntbl==2 ) {
        l_inv_predecode = 0.5;
        wire_cap = Cwordmetal*l_inv_predecode*total_edge_length;
        wire_res = 0.5*Rwordmetal*l_inv_predecode*total_edge_length;
    }

    if(Ntwl*Ntbl>2) {
        htree=logtwo((double)(Ntwl*Ntbl));
        htree_int = (int) htree;
        if (htree_int % 2 ==0) {
       		exp = (htree_int/2-1);
    	    l_inv_predecode = 0.25/(powers(2,exp));
            wire_cap = Cwordmetal*l_inv_predecode*total_edge_length;
            wire_res = 0.5*Rwordmetal*l_inv_predecode*total_edge_length;
        } else {
     		exp = (htree_int-3)/2;
           	l_inv_predecode = powers(2,exp);
           	wire_cap = Cbitmetal*l_inv_predecode*rows*8;
           	wire_res = 0.5*Rbitmetal*l_inv_predecode*rows*8;
        }
	}

	Cline = 4*gatecap(Wtdec3to8n+Wtdec3to8p,10.0)*(Ntwl*Ntbl)+ wire_cap;
	Cload = Cline / gatecap(1.0,0.0);

	Wtdecdriven1 = Cload*SizingRatio/3;
	Wtdecdrivep1 = 2*Cload*SizingRatio/3;

	// Size of second driver

	Wtdecdriven2 = (Wtdecdriven1+Wtdecdrivep1)*SizingRatio/3;
	Wtdecdrivep2 = 2*(Wtdecdriven1+Wtdecdrivep1)*SizingRatio/3;

}

void reset_powerDef(powerDef *power) {
	power->readOp.dynamic = 0.0;
	power->readOp.leakage = 0.0;

	power->writeOp.dynamic = 0.0;
	power->writeOp.leakage = 0.0;
}

void scale_powerDef(powerDef *power, int val) {
	power->readOp.dynamic *= val;
	power->readOp.leakage *= val;

	power->writeOp.dynamic *= val;
	power->writeOp.leakage *= val;
}

void copy_powerDef(powerDef *dest, powerDef source) {
	dest->readOp.dynamic = source.readOp.dynamic;
	dest->readOp.leakage = source.readOp.leakage;

	dest->writeOp.dynamic = source.writeOp.dynamic;
	dest->writeOp.leakage = source.writeOp.leakage;
}

void copy_scaled_powerDef(powerDef *dest, powerDef source, double val) {

	// only dynamic power needs to be scaled.
	dest->readOp.dynamic = source.readOp.dynamic/val;
	dest->readOp.leakage = source.readOp.leakage;

	dest->writeOp.dynamic = val*source.writeOp.dynamic;
	dest->writeOp.leakage = source.writeOp.leakage;
}

void add_powerDef_vals(powerDef *sum, powerDef a, powerDef b) {

	sum->readOp.dynamic = a.readOp.dynamic + b.readOp.dynamic;
	sum->readOp.leakage = a.readOp.leakage + b.readOp.leakage;

	sum->writeOp.dynamic = a.writeOp.dynamic + b.writeOp.dynamic;
	sum->writeOp.leakage = a.writeOp.leakage + b.writeOp.leakage;
}

/*======================================================================*/


void explore_configs(result_type *result,arearesult_type *arearesult,area_type *arearesult_subbanked,parameter_type *parameters,double *NSubbanks)
{
	extern int organizational_parameters_valid(int B, int A, int C, int Ndwl, int Ndbl, int Nspd,
				int Ntwl, int Ntbl, int Ntspd, char assoc);
	extern void area_subbanked (int baddr, int b0, int RWP, int ERP, int EWP, int Ndbl, int Ndwl,
				int Nspd, int Ntbl, int Ntwl, int Ntspd, double NSubbanks,
				parameter_type *parameters, area_type *result_subbanked, arearesult_type *result);
	void reset_device_widths();

	void reset_powerDef(powerDef *power);
	void scale_powerDef(powerDef *power, int val);
	void copy_powerDef(powerDef *dest, powerDef source);
	void copy_scaled_powerDef(powerDef *dest, powerDef source, double val);
	void add_powerDef_vals(powerDef *sum, powerDef a, powerDef b);

   	arearesult_type arearesult_temp;
   	area_type arearesult_subbanked_temp;

   	int Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,inter_subbanks,rows,columns,cols_subarray,rows_subarray,tag_driver_size1,tag_driver_size2;
   	int l_muxdrv_v, l_muxdrv_h, exp, htree_int;
   	double Subbank_Efficiency, Total_Efficiency, max_efficiency, efficiency, min_efficiency;
   	double max_aspect_ratio_total ,aspect_ratio_total_temp;
   	double htree;
   	double bank_h, bank_v, subbank_h, subbank_v, inter_h, inter_v;
   	double wirelength_v, wirelength_h, wirelength_sense_v, wirelength_sense_h;
   	double access_time=0;
   	//double total_power=0;
   	powerDef total_power;
   	double before_mux=0.0,after_mux=0;
   	// double total_address_routing_power=0.0, total_power_without_routing=0.0;
   	powerDef total_routing_power, total_power_allbanks;
   	powerDef total_power_without_routing;

   	double subbankaddress_routing_delay=0.0;
   	powerDef subbankaddress_routing_power;
   	double decoder_data_driver=0.0,decoder_data_3to8=0.0,decoder_data_inv=0.0;
   	double decoder_data=0.0,decoder_tag=0.0,wordline_data=0.0,wordline_tag=0.0;
   	// double darray_decoder_power=0.0,tarray_decoder_power=0.0,darray_wordline_power=0.0,tarray_wordline_power=0.0;
   	powerDef darray_decoder_power,tarray_decoder_power,darray_wordline_power,tarray_wordline_power;
   	double decoder_tag_driver=0.0,decoder_tag_3to8=0.0,decoder_tag_inv=0.0;
   	powerDef darray_readControl_power, tarray_readControl_power;
   	powerDef darray_writeControl_power, tarray_writeControl_power;
   	double bitline_data=0.0,bitline_tag=0.0,sense_amp_data=0.0,sense_amp_tag=0.0;
   	// double darray_bitline_power=0.0,tarray_bitline_power=0.0,darray_senseamp_power=0.0,tarray_senseamp_power=0.0;
   	powerDef darray_bitline_power,tarray_bitline_power,darray_senseamp_power,tarray_senseamp_power;
   	double compare_tag=0.0,mux_driver=0.0,data_output=0.0,selb=0.0;
   	//double compare_tag_power=0.0, selb_power=0.0, mux_driver_power=0.0, valid_driver_power=0.0;
   	powerDef compare_tag_power, selb_power, mux_driver_power, valid_driver_power;
   	// double data_output_power=0.0;
   	powerDef data_output_power, datain_driver_power;
   	double time_till_compare=0.0,time_till_select=0.0,driver_cap=0.0,valid_driver=0.0;
   	double cycle_time=0.0, precharge_del=0.0;
   	double outrisetime=0.0,inrisetime=0.0,addr_inrisetime=0.0;
   	double senseext_scale=1.0;
   	double total_out_driver=0.0;
   	// double total_out_driver_power=0.0;
   	powerDef total_out_driver_power;
   	float scale_init;
   	int data_nor_inputs=1, tag_nor_inputs=1;
   	double tag_delay=0.0, tag_delay_part1=0.0, tag_delay_part2=0.0, tag_delay_part3=0.0, tag_delay_part4=0.0,tag_delay_part5=0.0,tag_delay_part6=0.0;
   	double max_delay=0;
   	int i, hsConfig[6], lpConfig[6], aeConfig[6];
   	double hsTime, hsArea, lpTime, lpArea, aeArea, aeTime;
   	powerDef hsPower, lpPower, aePower;
   	double tarray_read_power, tarray_write_power, darray_read_power, darray_write_power;
   	int counter;
   	double Tpre,Tpre_tag;

   	rows = parameters->number_of_sets;
   	columns = 8*parameters->block_size*parameters->associativity;
   	FUDGEFACTOR=parameters->fudgefactor;

   	// VddPow=4.5/(pow(FUDGEFACTOR,(2.0/3.0)));
   	VddPow = Vdd_init;

   	if (VddPow < 0.7) VddPow=0.7;

   	if (VddPow > 5.0) VddPow=5.0;

   	VbitprePow=VddPow*3.3/4.5;
   	parameters->VddPow=VddPow;

   	/* go through possible Ndbl,Ndwl and find the smallest */
   	/* Because of area considerations, I don't think it makes sense
   	   to break either dimension up larger than MAXN */

   	/* only try moving output drivers for set associative cache */
   	if (parameters->associativity!=1)
   		scale_init=0.1;
   	else
   		scale_init=1.0;

   	if (parameters->fully_assoc)
   	/* If model is a fully associative cache - use larger cell size */
    {
    	FACbitmetal=((32+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1)+parameters->num_read_ports))*Cmetal);
     	FACwordmetal=((8+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1))+WIREPITCH*(parameters->num_read_ports))*Cmetal);
       	FARbitmetal=(((32+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1)+parameters->num_read_ports))/WIREWIDTH)*Rmetal);
       	FARwordmetal=(((8+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1))+WIREPITCH*(parameters->num_read_ports))/WIREWIDTH)*Rmetal);
    }

   	Cbitmetal=((16+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1)+parameters->num_read_ports))*Cmetal);
   	Cwordmetal=((8+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1))+WIREPITCH*(parameters->num_read_ports))*Cmetal);
   	Rbitmetal=(((16+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1)+parameters->num_read_ports))/WIREWIDTH)*Rmetal);
   	Rwordmetal=(((8+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1))+WIREPITCH*(parameters->num_read_ports))/WIREWIDTH)*Rmetal);

  	result->cycle_time = BIGNUM;
  	result->access_time = BIGNUM;
  	result->total_power.readOp.dynamic = BIGNUM;
  	result->total_power.readOp.leakage = BIGNUM;
  	result->total_power.writeOp.dynamic = BIGNUM;
  	result->total_power.writeOp.leakage = BIGNUM;
  	result->best_muxover=8*parameters->block_size/BITOUT;
  	result->max_access_time = 0;
  	result->min_access_time = BIGNUM;
  	reset_powerDef(&result->max_power);
	result->min_power.readOp.dynamic = BIGNUM; result->min_power.readOp.leakage = BIGNUM;
	result->min_power.writeOp.dynamic = BIGNUM; result->min_power.writeOp.leakage = BIGNUM;

  	arearesult_temp.max_efficiency = 1.0/BIGNUM;
  	arearesult_temp.min_efficiency = BIGNUM;
  	max_efficiency = 1.0/BIGNUM;
  	min_efficiency = BIGNUM;
  	arearesult->efficiency = 1.0/BIGNUM;
  	max_aspect_ratio_total = 1.0/BIGNUM;
  	arearesult->aspect_ratio_total = BIGNUM;

 	for (counter=0; counter<2; counter++) {
 		if (!parameters->fully_assoc) {
 	   		/* Set associative or direct map cache model */
 			for (Nspd=1;Nspd<=MAXSPD;Nspd=Nspd*2) {
    			for (Ndwl=1;Ndwl<=MAXN;Ndwl=Ndwl*2) {
    				for (Ndbl=1;Ndbl<=MAXN;Ndbl=Ndbl*2) {
    					for (Ntspd=1;Ntspd<=MAXSPD;Ntspd=Ntspd*2) {
       						for (Ntwl=1;Ntwl<=MAXN;Ntwl=Ntwl*2) {
								for (Ntbl=1;Ntbl<=MAXN;Ntbl=Ntbl*2) {
									if (organizational_parameters_valid
	      								(parameters->block_size, parameters->associativity, parameters->cache_size,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,parameters->fully_assoc)) {

	    								bank_h=0;
	    								bank_v=0;

        								if (8*parameters->block_size/BITOUT == 1 && Ndbl*Nspd==1) {
        								   muxover=1;
        								}
        								else {
        									if (Ndbl*Nspd > MAX_COL_MUX) {
        								    	muxover=8*parameters->block_size/BITOUT;
          									}
          									else {
          										if (8*parameters->block_size*Ndbl*Nspd/BITOUT > MAX_COL_MUX) {
                									muxover=(8*parameters->block_size/BITOUT)/(MAX_COL_MUX/(Ndbl*Nspd));
              									}
            									else  {
                									muxover=1;
              									}
	  										}
      									}

      									area_subbanked(ADDRESS_BITS,BITOUT,parameters->num_readwrite_ports,parameters->num_read_ports,parameters->num_write_ports,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,parameters,&arearesult_subbanked_temp,&arearesult_temp);
										Subbank_Efficiency= (area_all_dataramcells+area_all_tagramcells)*100/(arearesult_temp.totalarea/100000000.0);
       									Total_Efficiency= (*NSubbanks)*(area_all_dataramcells+area_all_tagramcells)*100/(calculate_area(arearesult_subbanked_temp,parameters->fudgefactor)/100000000.0);

	   									//efficiency = Subbank_Efficiency;
            							efficiency = Total_Efficiency;
	    								arearesult_temp.efficiency = efficiency;
        								aspect_ratio_total_temp = (arearesult_subbanked_temp.height/arearesult_subbanked_temp.width);
        								aspect_ratio_total_temp = (aspect_ratio_total_temp > 1.0) ? (aspect_ratio_total_temp) : 1.0/(aspect_ratio_total_temp) ;
	    								arearesult_temp.aspect_ratio_total = aspect_ratio_total_temp;

	    								reset_device_widths();

										compute_device_widths(parameters->cache_size, parameters->block_size, parameters->associativity,parameters->fully_assoc,Ndwl,Ndbl,Nspd);
						  				compute_tag_device_widths(parameters->cache_size, parameters->block_size, parameters->associativity,
						  											Ndwl,Ndbl,Nspd,Ntspd,Ntwl,Ntbl,(*NSubbanks));

	    								subbank_dim(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,&bank_h,&bank_v);
	    								//subbanks_routing_power(parameters->fully_assoc,parameters->associativity,*NSubbanks,&bank_h,&bank_v,&total_address_routing_power);
	    								//total_address_routing_power.dynamic *=(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports);
										//total_address_routing_power.leakage *=(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports);

										reset_powerDef(&subbankaddress_routing_power);
										reset_powerDef(&darray_decoder_power);
										reset_powerDef(&darray_wordline_power);
										reset_powerDef(&darray_bitline_power);
										reset_powerDef(&data_output_power);
										reset_powerDef(&darray_senseamp_power);
										reset_powerDef(&darray_readControl_power);
										reset_powerDef(&darray_writeControl_power);
										reset_powerDef(&total_out_driver_power);
										reset_powerDef(&datain_driver_power);

	    								outrisetime = 0.0;


										if(*NSubbanks > 2 ) {
	    									subbankaddress_routing_delay= compute_address_routing_data(parameters->cache_size, parameters->block_size, parameters->associativity, parameters->fully_assoc, Ndwl, Ndbl, Nspd, Ntwl, Ntbl,Ntspd,*NSubbanks,&outrisetime,&subbankaddress_routing_power);
										}

            							scale_powerDef(&subbankaddress_routing_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

            							/* Calculate data side of cache */
            							Tpre = 0;
            							inrisetime = outrisetime;
	    								addr_inrisetime=outrisetime;
	    								max_delay=0;

	    								decoder_data = compute_decoder_data(parameters->cache_size,parameters->block_size,
										parameters->associativity,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd, *NSubbanks,
					 						&decoder_data_driver,&decoder_data_3to8,
					 						&decoder_data_inv,inrisetime,&outrisetime, &data_nor_inputs,
					 						&darray_decoder_power);
					 					scale_powerDef(&darray_decoder_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

					 					Tpre = decoder_data;
	    								max_delay=MAX(max_delay, decoder_data);
	    								inrisetime = outrisetime;

	    								wordline_data = compute_wordline_data(parameters->cache_size, parameters->block_size,
											parameters->associativity,Ndwl,Ndbl,Nspd,
					   						inrisetime,&outrisetime,&darray_wordline_power);
					   					scale_powerDef(&darray_wordline_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

										Tpre += wordline_data;
	    								inrisetime = outrisetime;
	    								max_delay=MAX(max_delay, wordline_data);


	    								bitline_data = compute_bitline_data(parameters->cache_size,parameters->associativity,
											parameters->block_size,Ndwl,Ndbl,Nspd,
					 						inrisetime,&outrisetime, &darray_bitline_power, Tpre);
	    								scale_powerDef(&darray_bitline_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

	    								inrisetime = outrisetime;
	    								max_delay=MAX(max_delay, bitline_data);

	    								//int temp_row;
	      								compute_read_control_data(parameters->cache_size,parameters->associativity,
											parameters->block_size,Ndwl,Ndbl,Nspd, *NSubbanks,&darray_readControl_power);
										compute_write_control_data(parameters->cache_size,parameters->associativity,
											parameters->block_size,Ndwl,Ndbl,Nspd, *NSubbanks,&darray_writeControl_power);
	      								//temp_row = parameters->cache_size/(parameters->block_size*parameters->associativity*Ndbl*Nspd);
	      								sense_amp_data = compute_sense_amp_data(parameters->cache_size,parameters->associativity,
											parameters->block_size,Ndwl,Ndbl,Nspd,inrisetime,&outrisetime, &darray_senseamp_power);

	      								scale_powerDef(&darray_senseamp_power, (parameters->num_readwrite_ports+parameters->num_read_ports));
	      								scale_powerDef(&darray_readControl_power, (parameters->num_readwrite_ports+parameters->num_read_ports));
	      								scale_powerDef(&darray_writeControl_power, (parameters->num_readwrite_ports+parameters->num_read_ports));
	      								max_delay=MAX(max_delay, sense_amp_data);

	    								inrisetime = outrisetime;

	    								data_output = compute_dataoutput_data(parameters->cache_size,parameters->block_size,
											parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,
					   						inrisetime,&outrisetime, &data_output_power);
	    								scale_powerDef(&data_output_power,(parameters->num_readwrite_ports+parameters->num_read_ports));
	    								max_delay=MAX(max_delay, data_output);
	    								inrisetime = outrisetime;

	    								compute_datain_power(parameters->cache_size,parameters->block_size,
													parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,
													&datain_driver_power);
	    								scale_powerDef(&datain_driver_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

	    								subbank_v=0;
	    								subbank_h=0;

	    								subbank_routing_length(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,Ntbl,Ntwl,Ntspd,*NSubbanks,&subbank_v,&subbank_h) ;


	    								if(*NSubbanks > 2 ) {
											total_out_driver = compute_total_dataout_data(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,Ntbl,Ntspd,inrisetime,&outrisetime, subbank_v, subbank_h, &total_out_driver_power);
	    								}

										/*
							               total_out_driver = total_out_driver_delay(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,Ntbl,Ntwl,Ntspd,*NSubbanks,inrisetime,&outrisetime, &total_out_driver_power);
										*/
							            scale_powerDef(&total_out_driver_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

							            inrisetime = outrisetime;
							            max_delay=MAX(max_delay, total_out_driver);

										add_powerDef_vals(&total_routing_power, total_out_driver_power, subbankaddress_routing_power);

										/* if the associativity is 1, the data output can come right
	       								after the sense amp.   Otherwise, it has to wait until
	       								the data access has been done. */

	    								if (parameters->associativity==1) {
	    									before_mux = subbankaddress_routing_delay +decoder_data + wordline_data + bitline_data +
															sense_amp_data + total_out_driver + data_output;
	    									after_mux = 0;
	    								} else {
	    									before_mux = subbankaddress_routing_delay+ decoder_data + wordline_data + bitline_data +
															sense_amp_data;
	    									after_mux = data_output + total_out_driver;
	    								}

	    								/*
	    								 * Now worry about the tag side.
	    								 */

	    								reset_powerDef(&tarray_decoder_power);
	    								reset_powerDef(&tarray_wordline_power);
	    								reset_powerDef(&tarray_bitline_power);
	    								reset_powerDef(&tarray_senseamp_power);
	    								reset_powerDef(&tarray_readControl_power);
	    								reset_powerDef(&tarray_writeControl_power);

	    								decoder_tag = compute_decoder_tag_data(parameters->cache_size,
										    parameters->block_size,parameters->associativity,
										    Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,*NSubbanks,
										    &decoder_tag_driver,&decoder_tag_3to8,
										    &decoder_tag_inv,addr_inrisetime,&outrisetime,&tag_nor_inputs, &tarray_decoder_power);
	    								max_delay=MAX(max_delay, decoder_tag);
	    								inrisetime = outrisetime;
	    								scale_powerDef(&tarray_decoder_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));
	    								Tpre_tag = decoder_tag;


	    								wordline_tag = compute_wordline_tag_data(parameters->cache_size,parameters->block_size,
											parameters->associativity,Ndwl, Nspd, Ntspd,Ntwl,Ntbl,*NSubbanks,
					      					inrisetime,&outrisetime, &tarray_wordline_power);
	    								max_delay=MAX(max_delay, wordline_tag);
	    								Tpre_tag += wordline_tag;
	    								inrisetime = outrisetime;
	    								scale_powerDef(&tarray_wordline_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

	    								bitline_tag = compute_bitline_tag_data(parameters->cache_size,parameters->associativity,
										    parameters->block_size,Ntwl,Ntbl,Ntspd,*NSubbanks,
										    inrisetime,&outrisetime, &tarray_bitline_power,Tpre_tag);
	    								max_delay=MAX(max_delay, bitline_tag);
	    								inrisetime = outrisetime;
	    								scale_powerDef(&tarray_bitline_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

	    								sense_amp_tag = compute_sense_amp_tag_data(parameters->cache_size,parameters->associativity,
										    parameters->block_size,Ntwl,Ntbl,Ntspd,*NSubbanks,
										    inrisetime,&outrisetime,&tarray_senseamp_power);
	    								scale_powerDef(&tarray_senseamp_power,(ADDRESS_BITS+2 - (int)logtwo((double)parameters->cache_size) + (int)logtwo((double)parameters->associativity)));

	    								max_delay=MAX(max_delay, sense_amp_tag);
	    								inrisetime = outrisetime;
	    								scale_powerDef(&tarray_senseamp_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

										compute_tag_read_control_data(parameters->cache_size,parameters->associativity,
												parameters->block_size,Ntwl,Ntbl,Ntspd, *NSubbanks,&tarray_readControl_power);
										compute_tag_write_control_data(parameters->cache_size,parameters->associativity,
												parameters->block_size,Ntwl,Ntbl,Ntspd, *NSubbanks,&tarray_writeControl_power);

	    								/* split comparator - look at half the address bits only */
	    								reset_powerDef(&compare_tag_power);
	    								compare_tag = compute_half_compare_data(parameters->cache_size,parameters->associativity,
										    Ntbl,Ntspd,*NSubbanks,
										    inrisetime,&outrisetime, &compare_tag_power);
	    								scale_powerDef(&compare_tag_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

	    								inrisetime = outrisetime;
	    								max_delay=MAX(max_delay, compare_tag);

	    								reset_powerDef(&valid_driver_power);
	    								reset_powerDef(&mux_driver_power);
	    								reset_powerDef(&selb_power);

	    								if (parameters->associativity == 1) {
	    									mux_driver = 0;
	    									valid_driver = compute_valid_driver_data(parameters->cache_size,parameters->block_size,
															parameters->associativity,parameters->fully_assoc,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,inrisetime,&valid_driver_power);
											scale_powerDef(&valid_driver_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

	      									max_delay=MAX(max_delay, valid_driver);
	      									time_till_compare = subbankaddress_routing_delay+ decoder_tag + wordline_tag + bitline_tag +
												sense_amp_tag;
	      									time_till_select = time_till_compare+ compare_tag + valid_driver;


	      									/*
	      									 * From the above info, calculate the total access time
	      									 */

	      									access_time = MAX(before_mux+after_mux,time_till_select);
	    								} else {
	    									double wirelength;

	      									/* default scale is full wirelength */
        									cols_subarray = (8*parameters->block_size*parameters->associativity*Nspd/Ndwl);
											rows_subarray = (parameters->cache_size/(parameters->block_size*parameters->associativity*Ndbl*Nspd));

        									if(Ndwl*Ndbl==1) {
        								       	l_muxdrv_v= 0;
                   								l_muxdrv_h= cols_subarray;
            								}
            								if(Ndwl*Ndbl==2 || Ndwl*Ndbl==4) {
            								   	l_muxdrv_v= 0;
            								   	l_muxdrv_h= 2*cols_subarray;
            								}

            								if(Ndwl*Ndbl>4) {
                								htree=logtwo((double)(Ndwl*Ndbl));
                								htree_int = (int) htree;
                								if (htree_int % 2 ==0) {
                  									exp = (htree_int/2-1);
                  									l_muxdrv_v = (powers(2,exp)-1)*rows_subarray;
                									l_muxdrv_h = sqrt(Ndwl*Ndbl)*cols_subarray;
                								}
					            			    else {
						        			        exp = (htree_int+1)/2-1;
                  									l_muxdrv_v = (powers(2,exp)-1)*rows_subarray;
                									l_muxdrv_h = sqrt(Ndwl*Ndbl/2)*cols_subarray;
                								}
          									}

	      									wirelength_v=(l_muxdrv_v);
              								wirelength_h=(l_muxdrv_h);

			      							/* dualin mux driver - added for split comparator
		 									- inverter replaced by nand gate */

	      									mux_driver = compute_mux_driver_dualin_data(parameters->cache_size,
						   						parameters->block_size,
						   						parameters->associativity,
						   						Ndbl,Nspd,Ndwl,Ntbl,Ntspd,
						   						inrisetime,&outrisetime,
							   					wirelength_v,wirelength_h,&mux_driver_power);
	      									max_delay=MAX(max_delay, mux_driver);
	      									scale_powerDef(&mux_driver_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

	      									selb = compute_selb_tag_path_data(inrisetime,&outrisetime,&selb_power);
	      									max_delay=MAX(max_delay, selb);
	      									scale_powerDef(&selb_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

	      									valid_driver = 0;
	      									time_till_compare = subbankaddress_routing_delay+decoder_tag + wordline_tag + bitline_tag +sense_amp_tag;

	    					  				time_till_select = time_till_compare+ compare_tag + mux_driver+ selb;
	    									access_time = MAX(before_mux,time_till_select) +after_mux;
	    								}

	    								/*
	    								 * Calcuate the cycle time
	    								 */

	    								// precharge_del = precharge_delay(wordline_data);

	    								// cycle_time = access_time + precharge_del;

	    								cycle_time=access_time/WAVE_PIPE;
	    								if (max_delay>cycle_time)
	    									cycle_time=max_delay;

	    								/*
	    								 * The parameters are for a 0.8um process.  A quick way to
	    								 * scale the results to another process is to divide all
	    								 * the results by FUDGEFACTOR.  Normally, FUDGEFACTOR is 1.
	    								 */

										// Changed by MnM // felt no need for this.. (doesnt make sense)
	    								// darray_senseamp_power+=(data_output + total_out_driver)*500e-6*5;
	    								// if (!parameters->fully_assoc)
        								// 	tarray_senseamp_power+=(time_till_compare+ compare_tag + mux_driver+ selb +data_output + total_out_driver)*500e-6*5;

        								tarray_read_power = (tarray_decoder_power.readOp.dynamic+tarray_wordline_power.readOp.dynamic+tarray_bitline_power.readOp.dynamic+tarray_senseamp_power.readOp.dynamic+compare_tag_power.readOp.dynamic+valid_driver_power.readOp.dynamic+mux_driver_power.readOp.dynamic+selb_power.readOp.dynamic+tarray_readControl_power.readOp.dynamic+tarray_writeControl_power.readOp.dynamic)/FUDGEFACTOR +
        												    tarray_decoder_power.readOp.leakage+tarray_wordline_power.readOp.leakage+tarray_bitline_power.readOp.leakage+tarray_senseamp_power.readOp.leakage+compare_tag_power.readOp.leakage+valid_driver_power.readOp.leakage+mux_driver_power.readOp.leakage+selb_power.readOp.leakage+tarray_readControl_power.readOp.leakage+tarray_writeControl_power.readOp.leakage;

        								darray_read_power =	(subbankaddress_routing_power.readOp.dynamic+darray_decoder_power.readOp.dynamic+darray_wordline_power.readOp.dynamic+darray_bitline_power.readOp.dynamic+darray_senseamp_power.readOp.dynamic+total_out_driver_power.readOp.dynamic+data_output_power.readOp.dynamic+darray_readControl_power.readOp.dynamic+darray_writeControl_power.readOp.dynamic+datain_driver_power.readOp.dynamic)/FUDGEFACTOR+
        													subbankaddress_routing_power.readOp.leakage+darray_decoder_power.readOp.leakage+darray_wordline_power.readOp.leakage+darray_bitline_power.readOp.leakage+darray_senseamp_power.readOp.leakage+total_out_driver_power.readOp.leakage+data_output_power.readOp.leakage+darray_readControl_power.readOp.leakage+darray_writeControl_power.readOp.leakage+datain_driver_power.readOp.leakage;

        								tarray_write_power = (tarray_decoder_power.writeOp.dynamic+tarray_wordline_power.writeOp.dynamic+tarray_bitline_power.writeOp.dynamic+tarray_senseamp_power.writeOp.dynamic+compare_tag_power.writeOp.dynamic+valid_driver_power.writeOp.dynamic+mux_driver_power.writeOp.dynamic+selb_power.writeOp.dynamic+tarray_readControl_power.writeOp.dynamic+tarray_writeControl_power.writeOp.dynamic)/FUDGEFACTOR +
        												    tarray_decoder_power.writeOp.leakage+tarray_wordline_power.writeOp.leakage+tarray_bitline_power.writeOp.leakage+tarray_senseamp_power.writeOp.leakage+compare_tag_power.writeOp.leakage+valid_driver_power.writeOp.leakage+mux_driver_power.writeOp.leakage+selb_power.writeOp.leakage+tarray_readControl_power.writeOp.leakage+tarray_writeControl_power.writeOp.leakage;

        								darray_write_power = (subbankaddress_routing_power.writeOp.dynamic+darray_decoder_power.writeOp.dynamic+darray_wordline_power.writeOp.dynamic+darray_bitline_power.writeOp.dynamic+darray_senseamp_power.writeOp.dynamic+total_out_driver_power.writeOp.dynamic+data_output_power.writeOp.dynamic+darray_readControl_power.writeOp.dynamic+darray_writeControl_power.writeOp.dynamic+datain_driver_power.writeOp.dynamic)/FUDGEFACTOR+
        													subbankaddress_routing_power.writeOp.leakage+darray_decoder_power.writeOp.leakage+darray_wordline_power.writeOp.leakage+darray_bitline_power.writeOp.leakage+darray_senseamp_power.writeOp.leakage+total_out_driver_power.writeOp.leakage+data_output_power.writeOp.leakage+darray_readControl_power.writeOp.leakage+darray_writeControl_power.writeOp.leakage+datain_driver_power.writeOp.leakage;

        								total_power.readOp.dynamic =(subbankaddress_routing_power.readOp.dynamic+darray_decoder_power.readOp.dynamic+darray_wordline_power.readOp.dynamic+darray_bitline_power.readOp.dynamic+darray_senseamp_power.readOp.dynamic+total_out_driver_power.readOp.dynamic+tarray_decoder_power.readOp.dynamic+tarray_wordline_power.readOp.dynamic+tarray_bitline_power.readOp.dynamic+tarray_senseamp_power.readOp.dynamic+compare_tag_power.readOp.dynamic+valid_driver_power.readOp.dynamic+mux_driver_power.readOp.dynamic+selb_power.readOp.dynamic+data_output_power.readOp.dynamic+darray_readControl_power.readOp.dynamic+tarray_readControl_power.readOp.dynamic+darray_writeControl_power.readOp.dynamic+tarray_writeControl_power.readOp.dynamic+datain_driver_power.readOp.dynamic)/FUDGEFACTOR;
										total_power_without_routing.readOp.dynamic =(*NSubbanks)*(darray_decoder_power.readOp.dynamic+darray_wordline_power.readOp.dynamic+darray_bitline_power.readOp.dynamic+darray_senseamp_power.readOp.dynamic+tarray_decoder_power.readOp.dynamic+tarray_wordline_power.readOp.dynamic+tarray_bitline_power.readOp.dynamic+tarray_senseamp_power.readOp.dynamic+compare_tag_power.readOp.dynamic+mux_driver_power.readOp.dynamic+selb_power.readOp.dynamic+data_output_power.readOp.dynamic+darray_readControl_power.readOp.dynamic+tarray_readControl_power.readOp.dynamic+darray_writeControl_power.readOp.dynamic+tarray_writeControl_power.readOp.dynamic)/FUDGEFACTOR+valid_driver_power.readOp.dynamic/FUDGEFACTOR;
										total_power_allbanks.readOp.dynamic = total_power_without_routing.readOp.dynamic + total_routing_power.readOp.dynamic/FUDGEFACTOR;

										total_power.readOp.leakage =(subbankaddress_routing_power.readOp.leakage+darray_decoder_power.readOp.leakage+darray_wordline_power.readOp.leakage+darray_bitline_power.readOp.leakage+darray_senseamp_power.readOp.leakage+total_out_driver_power.readOp.leakage+tarray_decoder_power.readOp.leakage+tarray_wordline_power.readOp.leakage+tarray_bitline_power.readOp.leakage+tarray_senseamp_power.readOp.leakage+compare_tag_power.readOp.leakage+valid_driver_power.readOp.leakage+mux_driver_power.readOp.leakage+selb_power.readOp.leakage+data_output_power.readOp.leakage+darray_readControl_power.readOp.leakage+tarray_readControl_power.readOp.leakage+darray_writeControl_power.readOp.leakage+tarray_writeControl_power.readOp.leakage+datain_driver_power.readOp.leakage);
										total_power_without_routing.readOp.leakage =(*NSubbanks)*(darray_decoder_power.readOp.leakage+darray_wordline_power.readOp.leakage+darray_bitline_power.readOp.leakage+darray_senseamp_power.readOp.leakage+tarray_decoder_power.readOp.leakage+tarray_wordline_power.readOp.leakage+tarray_bitline_power.readOp.leakage+tarray_senseamp_power.readOp.leakage+compare_tag_power.readOp.leakage+mux_driver_power.readOp.leakage+selb_power.readOp.leakage+data_output_power.readOp.leakage+darray_readControl_power.readOp.leakage+tarray_readControl_power.readOp.leakage+darray_writeControl_power.readOp.leakage+tarray_writeControl_power.readOp.leakage)+valid_driver_power.readOp.leakage;
										total_power_allbanks.readOp.leakage = total_power_without_routing.readOp.leakage + total_routing_power.readOp.leakage;

										total_power.writeOp.dynamic =(subbankaddress_routing_power.writeOp.dynamic+darray_decoder_power.writeOp.dynamic+darray_wordline_power.writeOp.dynamic+darray_bitline_power.writeOp.dynamic+darray_senseamp_power.writeOp.dynamic+total_out_driver_power.writeOp.dynamic+data_output_power.writeOp.dynamic+darray_writeControl_power.writeOp.dynamic+datain_driver_power.writeOp.dynamic+darray_readControl_power.writeOp.dynamic+tarray_decoder_power.readOp.dynamic+tarray_wordline_power.readOp.dynamic+tarray_bitline_power.readOp.dynamic+tarray_senseamp_power.readOp.dynamic+compare_tag_power.readOp.dynamic+valid_driver_power.readOp.dynamic+mux_driver_power.readOp.dynamic+selb_power.readOp.dynamic+tarray_readControl_power.readOp.dynamic+tarray_writeControl_power.readOp.dynamic)/FUDGEFACTOR;
										total_power_without_routing.writeOp.dynamic =(*NSubbanks)*(darray_decoder_power.writeOp.dynamic+darray_wordline_power.writeOp.dynamic+darray_bitline_power.writeOp.dynamic+darray_senseamp_power.writeOp.dynamic+tarray_decoder_power.writeOp.dynamic+tarray_wordline_power.writeOp.dynamic+tarray_bitline_power.writeOp.dynamic+tarray_senseamp_power.writeOp.dynamic+compare_tag_power.writeOp.dynamic+mux_driver_power.writeOp.dynamic+selb_power.writeOp.dynamic+data_output_power.writeOp.dynamic+darray_readControl_power.writeOp.dynamic+tarray_readControl_power.writeOp.dynamic+darray_writeControl_power.writeOp.dynamic+tarray_writeControl_power.writeOp.dynamic)/FUDGEFACTOR+valid_driver_power.readOp.dynamic/FUDGEFACTOR;
										total_power_allbanks.writeOp.dynamic = total_power_without_routing.writeOp.dynamic + total_routing_power.writeOp.dynamic/FUDGEFACTOR;

										total_power.writeOp.leakage =(subbankaddress_routing_power.writeOp.leakage+darray_decoder_power.writeOp.leakage+darray_wordline_power.writeOp.leakage+darray_bitline_power.writeOp.leakage+darray_senseamp_power.writeOp.leakage+total_out_driver_power.writeOp.leakage+data_output_power.writeOp.leakage+darray_writeControl_power.writeOp.leakage+datain_driver_power.writeOp.leakage+darray_readControl_power.writeOp.leakage+tarray_decoder_power.readOp.leakage+tarray_wordline_power.readOp.leakage+tarray_bitline_power.readOp.leakage+tarray_senseamp_power.readOp.leakage+compare_tag_power.readOp.leakage+valid_driver_power.readOp.leakage+mux_driver_power.readOp.leakage+selb_power.readOp.leakage+tarray_readControl_power.readOp.leakage+tarray_writeControl_power.readOp.leakage)/FUDGEFACTOR;
										total_power_without_routing.writeOp.leakage =(*NSubbanks)*(darray_decoder_power.writeOp.leakage+darray_wordline_power.writeOp.leakage+darray_bitline_power.writeOp.leakage+darray_senseamp_power.writeOp.leakage+tarray_decoder_power.writeOp.leakage+tarray_wordline_power.writeOp.leakage+tarray_bitline_power.writeOp.leakage+tarray_senseamp_power.writeOp.leakage+compare_tag_power.writeOp.leakage+mux_driver_power.writeOp.leakage+selb_power.writeOp.leakage+data_output_power.writeOp.leakage+darray_readControl_power.writeOp.leakage+tarray_readControl_power.writeOp.leakage+darray_writeControl_power.writeOp.leakage+tarray_writeControl_power.writeOp.leakage)/FUDGEFACTOR+valid_driver_power.readOp.leakage/FUDGEFACTOR;
										total_power_allbanks.writeOp.leakage = total_power_without_routing.writeOp.leakage + total_routing_power.writeOp.leakage;

								//		if (counter == 1 && (verbose == 1 || (total_power.readOp.dynamic+total_power.readOp.leakage) == (result->min_power.readOp.leakage+result->min_power.readOp.dynamic)) ) {
								//			//fprintf(stderr, "(%d, %d, %d, %d, %d, %d) ",Ndwl, Nspd, Ndbl, Ntwl, Ntspd, Ntbl);
	    						//			//fprintf(stderr, "Pow - %f, Acc - %f, Pow - %f, Acc - %f, Combo - %f\n", (total_power.dynamic+total_power.leakage)*1e3, access_time*1e3, (total_power.dynamic+total_power.leakage)/(result->max_power.dynamic+result->max_power.leakage), access_time/result->max_access_time, (total_power.dynamic+total_power.leakage)*access_time/((result->max_power.dynamic+result->max_power.leakage)*result->max_access_time) );
	    						//			fprintf(stderr, " Cache read hit power = %5.4f mW\n Cache write hit power = %5.4f mW\n",(tarray_read_power+darray_read_power)*1e3, (tarray_read_power+darray_write_power)*1e3);
	    						//			// for a read miss, there wud be a tag read, data read, line fill (tag write and data write).
	    						//			// for a write miss, there wud be a tag read and line fill (tag write and data write)
	    						//			fprintf(stderr, " Cache read miss power = %5.4f mW\n Cache write miss power = %5.4f mW\n",(tarray_read_power+darray_read_power+tarray_write_power+darray_write_power)*1e3,(tarray_read_power+tarray_write_power+darray_write_power)*1e3);
	    						//			fprintf(stderr, " CacheReadAccessHit Power Contributions: (%d, %d, %d, %d, %d, %d) %f mW %fns \n", Ndwl, Nspd, Ndbl, Ntwl, Ntspd, Ntbl, (total_power.readOp.dynamic+total_power.readOp.leakage)*1e3, access_time/FUDGEFACTOR*1e9);
	    						//			fprintf(stderr, "   (%5.4f, %5.4f) (%7.6f, %7.6f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f)\n     (%5.4f, %5.4f) (%7.6f, %7.6f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f)\n     (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f)\n     (%7.6f %7.6f) (%7.6f %7.6f) (%7.6f %7.6f) (%5.4f, %5.4f) \n",

	    						//							darray_decoder_power.readOp.dynamic/FUDGEFACTOR*1e3, darray_decoder_power.readOp.leakage*1e3,
	    						//							darray_wordline_power.readOp.dynamic/FUDGEFACTOR*1e3, darray_wordline_power.readOp.leakage*1e3,
	    						//							darray_bitline_power.readOp.dynamic/FUDGEFACTOR*1e3, darray_bitline_power.readOp.leakage*1e3,
	    						//							darray_senseamp_power.readOp.dynamic/FUDGEFACTOR*1e3, darray_senseamp_power.readOp.leakage*1e3,
	    						//							darray_readControl_power.readOp.dynamic/FUDGEFACTOR*1e3, darray_readControl_power.readOp.leakage*1e3,
	    						//							darray_writeControl_power.readOp.dynamic/FUDGEFACTOR*1e3, darray_writeControl_power.readOp.leakage*1e3,

	    						//							tarray_decoder_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_decoder_power.readOp.leakage*1e3,
	    						//							tarray_wordline_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_wordline_power.readOp.leakage*1e3,
	    						//							tarray_bitline_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_bitline_power.readOp.leakage*1e3,
	    						//							tarray_senseamp_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_senseamp_power.readOp.leakage*1e3,
	    						//							tarray_readControl_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_readControl_power.readOp.leakage*1e3,
	    						//							tarray_writeControl_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_writeControl_power.readOp.leakage*1e3,
//
	    						//							compare_tag_power.readOp.dynamic/FUDGEFACTOR*1e3, compare_tag_power.readOp.leakage*1e3,
	    						//							valid_driver_power.readOp.dynamic/FUDGEFACTOR*1e3, valid_driver_power.readOp.leakage*1e3,
	    						//							mux_driver_power.readOp.dynamic/FUDGEFACTOR*1e3, mux_driver_power.readOp.leakage*1e3,
	    						//							selb_power.readOp.dynamic/FUDGEFACTOR*1e3, selb_power.readOp.leakage*1e3,
	    						//							subbankaddress_routing_power.readOp.dynamic/FUDGEFACTOR*1e3, subbankaddress_routing_power.readOp.leakage*1e3,
								//							total_out_driver_power.readOp.dynamic/FUDGEFACTOR*1e3, total_out_driver_power.readOp.leakage*1e3,
								//							datain_driver_power.readOp.dynamic/FUDGEFACTOR*1e3, datain_driver_power.readOp.leakage*1e3,
	    						//							data_output_power.readOp.dynamic/FUDGEFACTOR*1e3, data_output_power.readOp.leakage*1e3);
//
	    						//			fprintf(stderr, " CacheWriteAccessHit Power Contributions: (%d, %d, %d, %d, %d, %d) %f mW \n", Ndwl, Nspd, Ndbl, Ntwl, Ntspd, Ntbl, (total_power.writeOp.dynamic+total_power.writeOp.leakage)*1e3);
	    						//			fprintf(stderr, "   (%5.4f, %5.4f) (%7.6f, %7.6f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f)\n     (%5.4f, %5.4f) (%7.6f, %7.6f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f)\n     (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f)\n     (%7.6f %7.6f) (%7.6f %7.6f) (%7.6f %7.6f) (%5.4f, %5.4f) \n",
//
	    						//							darray_decoder_power.writeOp.dynamic/FUDGEFACTOR*1e3, darray_decoder_power.writeOp.leakage*1e3,
	    						//							darray_wordline_power.writeOp.dynamic/FUDGEFACTOR*1e3, darray_wordline_power.writeOp.leakage*1e3,
	    						//							darray_bitline_power.writeOp.dynamic/FUDGEFACTOR*1e3, darray_bitline_power.writeOp.leakage*1e3,
	    						//							darray_senseamp_power.writeOp.dynamic/FUDGEFACTOR*1e3, darray_senseamp_power.writeOp.leakage*1e3,
	    						//							darray_readControl_power.writeOp.dynamic/FUDGEFACTOR*1e3, darray_readControl_power.writeOp.leakage*1e3,
	    						//							darray_writeControl_power.writeOp.dynamic/FUDGEFACTOR*1e3, darray_writeControl_power.writeOp.leakage*1e3,
//
	    						//							tarray_decoder_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_decoder_power.readOp.leakage*1e3,
	    						//							tarray_wordline_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_wordline_power.readOp.leakage*1e3,
	    						//							tarray_bitline_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_bitline_power.readOp.leakage*1e3,
	    						//							tarray_senseamp_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_senseamp_power.readOp.leakage*1e3,
	    						//							tarray_readControl_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_readControl_power.readOp.leakage*1e3,
	    						//							tarray_writeControl_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_writeControl_power.readOp.leakage*1e3,
//
	    						//							compare_tag_power.readOp.dynamic/FUDGEFACTOR*1e3, compare_tag_power.readOp.leakage*1e3,
	    						//							valid_driver_power.readOp.dynamic/FUDGEFACTOR*1e3, valid_driver_power.readOp.leakage*1e3,
	    						//							mux_driver_power.readOp.dynamic/FUDGEFACTOR*1e3, mux_driver_power.readOp.leakage*1e3,
	    						//							selb_power.readOp.dynamic/FUDGEFACTOR*1e3, selb_power.readOp.leakage*1e3,
	    						//							subbankaddress_routing_power.writeOp.dynamic/FUDGEFACTOR*1e3, subbankaddress_routing_power.writeOp.leakage*1e3,
								//							total_out_driver_power.writeOp.dynamic/FUDGEFACTOR*1e3, total_out_driver_power.writeOp.leakage*1e3,
								//							datain_driver_power.writeOp.dynamic/FUDGEFACTOR*1e3, datain_driver_power.writeOp.leakage*1e3,
	    						//							data_output_power.writeOp.dynamic/FUDGEFACTOR*1e3, data_output_power.writeOp.leakage*1e3);
	    						//		}

	    								if (counter==1)	{

											// if ( ( (result->total_power.dynamic + result->total_power.leakage)/(result->max_power.leakage + result->max_power.dynamic) )/2
           									//	+(result->access_time/result->max_access_time)
           									//	> (( (total_power.dynamic+total_power.leakage)/(result->max_power.leakage + result->max_power.dynamic))/2
           									//		+ access_time/(result->max_access_time*FUDGEFACTOR)) )

		   									if (( (result->total_power.readOp.dynamic + result->total_power.readOp.leakage)/(result->max_power.readOp.dynamic + result->max_power.readOp.leakage) )*4
		   									 		+(result->access_time/result->max_access_time)*2
		   									 		+ (1.0/arearesult->efficiency)*4
		   									 		+ (arearesult->aspect_ratio_total/max_aspect_ratio_total)
		   									 		>
		   									 	(( (total_power.readOp.dynamic + total_power.readOp.leakage)/(result->max_power.readOp.dynamic + result->max_power.readOp.leakage))*4
		   									 		+ access_time/(result->max_access_time*FUDGEFACTOR)*2
		   									 		+ (1.0/efficiency)*4
		   									 		+ (arearesult_temp.aspect_ratio_total/max_aspect_ratio_total) ))


											//if (result->access_time+1e-11*(result->best_Ndwl+result->best_Ndbl+result->best_Nspd+result->best_Ntwl+result->best_Ntbl+result->best_Ntspd) > access_time/FUDGEFACTOR+1e-11*(Ndwl+Ndbl+Nspd+Ntwl+Ntbl+Ntspd))
		  									//if (result->access_time > access_time/FUDGEFACTOR)
											{
		  										result->senseext_scale = senseext_scale;

												copy_powerDef(&result->total_power, total_power);
												copy_powerDef(&result->total_power_without_routing, total_power_without_routing);
          										copy_powerDef(&result->total_power_allbanks, total_power_allbanks);

          										copy_scaled_powerDef(&result->total_routing_power, total_routing_power, FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->subbank_address_routing_power, subbankaddress_routing_power, FUDGEFACTOR);

		  										result->subbank_address_routing_delay = subbankaddress_routing_delay/FUDGEFACTOR;
		  										result->cycle_time = cycle_time/FUDGEFACTOR;
		  										result->access_time = access_time/FUDGEFACTOR;
		  										result->best_muxover = muxover;
		  										result->best_Ndwl = Ndwl;
		  										result->best_Ndbl = Ndbl;
		  										result->best_Nspd = Nspd;
		  										result->best_Ntwl = Ntwl;
		  										result->best_Ntbl = Ntbl;
		  										result->best_Ntspd = Ntspd;
		  										result->dec_tag_driver = decoder_tag_driver/FUDGEFACTOR;
												result->dec_tag_3to8 = decoder_tag_3to8/FUDGEFACTOR;
												result->dec_tag_inv = decoder_tag_inv/FUDGEFACTOR;
												result->dec_data_driver = decoder_data_driver/FUDGEFACTOR;
												result->dec_data_3to8 = decoder_data_3to8/FUDGEFACTOR;
												result->dec_data_inv = decoder_data_inv/FUDGEFACTOR;
												result->wordline_delay_data = wordline_data/FUDGEFACTOR;

		  										result->decoder_delay_data = decoder_data/FUDGEFACTOR;
		  										result->decoder_delay_tag = decoder_tag/FUDGEFACTOR;
		  										result->wordline_delay_tag = wordline_tag/FUDGEFACTOR;
		  										result->bitline_delay_data = bitline_data/FUDGEFACTOR;
		  										result->bitline_delay_tag = bitline_tag/FUDGEFACTOR;
		  										result->sense_amp_delay_data = sense_amp_data/FUDGEFACTOR;
		  										result->sense_amp_delay_tag = sense_amp_tag/FUDGEFACTOR;

		  										copy_scaled_powerDef(&result->decoder_power_data, darray_decoder_power, FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->decoder_power_tag,tarray_decoder_power,FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->wordline_power_data,darray_wordline_power,FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->wordline_power_tag,tarray_wordline_power,FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->bitline_power_data,darray_bitline_power,FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->bitline_power_tag,tarray_bitline_power,FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->sense_amp_power_data,darray_senseamp_power,FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->sense_amp_power_tag,tarray_senseamp_power,FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->total_out_driver_power_data,total_out_driver_power,FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->compare_part_power,compare_tag_power,FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->drive_mux_power,mux_driver_power,FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->selb_power,selb_power,FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->drive_valid_power,valid_driver_power,FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->data_output_power,data_output_power,FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->read_control_power_data,darray_readControl_power, FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->read_control_power_tag,tarray_readControl_power, FUDGEFACTOR);
		  										copy_scaled_powerDef(&result->write_control_power_data,darray_writeControl_power, FUDGEFACTOR);
												copy_scaled_powerDef(&result->write_control_power_tag,tarray_writeControl_power, FUDGEFACTOR);

												result->darray_read_power = darray_read_power;
												result->darray_write_power = darray_write_power;
												result->tarray_read_power = tarray_read_power;
												result->tarray_write_power = tarray_write_power;

												result->total_out_driver_delay_data = total_out_driver/FUDGEFACTOR;
		  										result->compare_part_delay = compare_tag/FUDGEFACTOR;
		  										result->drive_mux_delay = mux_driver/FUDGEFACTOR;
		  										result->selb_delay = selb/FUDGEFACTOR;
		  										result->drive_valid_delay = valid_driver/FUDGEFACTOR;
		  										result->data_output_delay = data_output/FUDGEFACTOR;
		  										result->precharge_delay = precharge_del/FUDGEFACTOR;

		  										result->data_nor_inputs = data_nor_inputs;
		  										result->tag_nor_inputs = tag_nor_inputs;
          								        area_subbanked(ADDRESS_BITS,BITOUT,parameters->num_readwrite_ports,parameters->num_read_ports,parameters->num_write_ports,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,parameters,arearesult_subbanked,arearesult);
 		  										arearesult->efficiency = (*NSubbanks)*(area_all_dataramcells+area_all_tagramcells)*100/(calculate_area(*arearesult_subbanked,parameters->fudgefactor)/100000000.0);
        								        arearesult->aspect_ratio_total = (arearesult_subbanked->height/arearesult_subbanked->width);
        										arearesult->aspect_ratio_total = (arearesult->aspect_ratio_total > 1.0) ? (arearesult->aspect_ratio_total) : 1.0/(arearesult->aspect_ratio_total) ;
												arearesult->max_efficiency = max_efficiency;
												arearesult->max_aspect_ratio_total = max_aspect_ratio_total;
											}
	      								}
	    								else
	    								{
											/* for optimal area, power and time */
											if (result->min_access_time > access_time/FUDGEFACTOR) {
												result->min_access_time = access_time/FUDGEFACTOR;
												hsConfig[0] = Ndwl; hsConfig[1] = Nspd; hsConfig[2] = Ndbl;
												hsConfig[3] = Ntwl; hsConfig[4] = Ntspd; hsConfig[5] =  Ntbl;
												hsTime = access_time/FUDGEFACTOR; hsArea = efficiency; copy_powerDef(&hsPower, total_power);
											}
											if ((result->min_power.readOp.dynamic + result->min_power.readOp.leakage) > (total_power.readOp.dynamic + total_power.readOp.leakage)) {
												copy_powerDef(&result->min_power,total_power);
												lpConfig[0] = Ndwl; lpConfig[1] = Nspd; lpConfig[2] = Ndbl;
												lpConfig[3] = Ntwl; lpConfig[4] = Ntspd; lpConfig[5] =  Ntbl;
												lpTime = access_time/FUDGEFACTOR; lpArea = efficiency; copy_powerDef(&lpPower, total_power);
											}
											if (result->max_access_time < access_time/FUDGEFACTOR)
												result->max_access_time = access_time/FUDGEFACTOR;

											if ((result->max_power.readOp.dynamic + result->max_power.readOp.leakage) < (total_power.readOp.dynamic + total_power.readOp.leakage)) {
												copy_powerDef(&result->max_power, total_power);
											}
											if (arearesult_temp.max_efficiency < efficiency) {
												max_efficiency = efficiency;
											}
											if (arearesult_temp.min_efficiency > efficiency) {
		   										arearesult_temp.min_efficiency = efficiency;
		   										min_efficiency = efficiency;
												aeConfig[0] = Ndwl; aeConfig[1] = Nspd; aeConfig[2] = Ndbl;
												aeConfig[3] = Ntwl; aeConfig[4] = Ntspd; aeConfig[5] =  Ntbl;
												aeTime = access_time/FUDGEFACTOR; aeArea = efficiency; copy_powerDef(&aePower, total_power);
		   									}
               								if (min_efficiency > efficiency)
               									min_efficiency = efficiency;
											if (max_aspect_ratio_total < aspect_ratio_total_temp)
												max_aspect_ratio_total = aspect_ratio_total_temp;
	  					    			}
	  								}
								}
     						}
     					}
    				}
   				}
   			}
		}
		else
		{
    		/* Fully associative model - only vary Ndbl|Ntbl */

			for (Ndbl=1;Ndbl<=MAXN;Ndbl=Ndbl*2)
			{
				Ntbl=Ndbl;
			    Ndwl=Nspd=Ntwl=Ntspd=1;

			    reset_device_widths();
			    outrisetime = 0.0;

			    if (organizational_parameters_valid
				   (parameters->block_size, 1, parameters->cache_size,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,parameters->fully_assoc))
				{

					if (8*parameters->block_size/BITOUT == 1 && Ndbl*Nspd==1)
					{
        			   muxover=1;
			        }
			        else
			        {
			        	if (Ndbl*Nspd > MAX_COL_MUX)
			        	{
			           		muxover=8*parameters->block_size/BITOUT;
			          	}
				      	else
				      	{
				       		if (8*parameters->block_size*Ndbl*Nspd/BITOUT > MAX_COL_MUX)
				       	   	{
               					muxover=(8*parameters->block_size/BITOUT)/(MAX_COL_MUX/(Ndbl*Nspd));
            				}
            				else
            				{
               					muxover=1;
            				}
          				}
        			}


        			area_subbanked(ADDRESS_BITS,BITOUT,parameters->num_readwrite_ports,parameters->num_read_ports,parameters->num_write_ports,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,parameters,&arearesult_subbanked_temp,&arearesult_temp);

        			Subbank_Efficiency= (area_all_dataramcells+area_all_tagramcells)*100/(arearesult_temp.totalarea/100000000.0);
        			Total_Efficiency= (*NSubbanks)*(area_all_dataramcells+area_all_tagramcells)*100/(calculate_area(arearesult_subbanked_temp,parameters->fudgefactor)/100000000.0);
        			// efficiency = Subbank_Efficiency;
        			efficiency = Total_Efficiency;
        			arearesult_temp.efficiency = efficiency;
        			aspect_ratio_total_temp = (arearesult_subbanked_temp.height/arearesult_subbanked_temp.width);
        			aspect_ratio_total_temp = (aspect_ratio_total_temp > 1.0) ? (aspect_ratio_total_temp) : 1.0/(aspect_ratio_total_temp) ;

        			arearesult_temp.aspect_ratio_total = aspect_ratio_total_temp;

					//compute_device_widths(parameters->cache_size, parameters->block_size, parameters->associativity,
					//	  					Ndwl,Ndbl,Nspd);
        			bank_h=0;
        			bank_v=0;
        			subbank_dim(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,&bank_h,&bank_v);
        			//subbanks_routing_power(parameters->fully_assoc,parameters->associativity,*NSubbanks,&bank_h,&bank_v,&total_address_routing_power);
        			//total_address_routing_power.dynamic *=(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports);
        			//total_address_routing_power.leakage *=(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports);

        			reset_powerDef(&subbankaddress_routing_power);
        			reset_powerDef(&darray_decoder_power);

					if(*NSubbanks > 2 )
        			{
        				subbankaddress_routing_delay= compute_address_routing_data(parameters->cache_size, parameters->block_size, parameters->associativity, parameters->fully_assoc, Ndwl, Ndbl, Nspd, Ntwl, Ntbl,Ntspd,*NSubbanks,&outrisetime,&subbankaddress_routing_power);
        			}

        			scale_powerDef(&subbankaddress_routing_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

        			/* Calculate data side of cache */
      				inrisetime = outrisetime;
        			addr_inrisetime=outrisetime;

	 				max_delay=0;

	 				/* tag path contained here */
	 				decoder_data = fa_tag_delay(parameters->cache_size,
						  parameters->block_size,
						  Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,
						  &tag_delay_part1,&tag_delay_part2,
						  &tag_delay_part3,&tag_delay_part4,
						  &tag_delay_part5,&tag_delay_part6,
						  &outrisetime,&tag_nor_inputs, &darray_decoder_power);
	 				scale_powerDef(&darray_decoder_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

	 				inrisetime = outrisetime;
	 				max_delay=MAX(max_delay, decoder_data);
					Tpre = decoder_data;

	 				reset_powerDef(&darray_wordline_power);
	 				wordline_data = compute_wordline_data(parameters->cache_size, parameters->block_size,1,Ndwl,Ndbl,Nspd,
					inrisetime,&outrisetime, &darray_wordline_power);
	 				scale_powerDef(&darray_wordline_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

	 				inrisetime = outrisetime;
	 				max_delay=MAX(max_delay, wordline_data);
	 				Tpre += wordline_data;

	 				reset_powerDef(&darray_bitline_power);
	 				bitline_data = compute_bitline_data(parameters->cache_size,1,
					      parameters->block_size,Ndwl,Ndbl,Nspd,
					      inrisetime,&outrisetime,&darray_bitline_power, Tpre);
	 				scale_powerDef(&darray_bitline_power, (parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

	 				inrisetime = outrisetime;
	 				max_delay=MAX(max_delay, bitline_data);
	 				reset_powerDef(&darray_senseamp_power);
	 				reset_powerDef(&darray_readControl_power);
	 				reset_powerDef(&darray_writeControl_power);
	 				{
	 					//int temp_row;
	 					compute_read_control_data(parameters->cache_size,parameters->associativity,
											parameters->block_size,Ndwl,Ndbl,Nspd, *NSubbanks,&darray_readControl_power);
						compute_write_control_data(parameters->cache_size,parameters->associativity,
											parameters->block_size,Ndwl,Ndbl,Nspd, *NSubbanks,&darray_writeControl_power);
	   					//temp_row = parameters->cache_size/(parameters->block_size*Ndbl);

	   					sense_amp_data = compute_sense_amp_data(parameters->cache_size,parameters->associativity,
											parameters->block_size,Ndwl,Ndbl,Nspd,inrisetime,&outrisetime, &darray_senseamp_power);
	   					scale_powerDef(&darray_senseamp_power,BITOUT*muxover/2);
	   					scale_powerDef(&darray_senseamp_power,(parameters->num_readwrite_ports+parameters->num_read_ports));
	   					scale_powerDef(&darray_readControl_power,(parameters->num_readwrite_ports+parameters->num_read_ports));
	      				scale_powerDef(&darray_writeControl_power,(parameters->num_readwrite_ports+parameters->num_read_ports));
	   					max_delay=MAX(max_delay, sense_amp_data);
	 				}

	 				inrisetime = outrisetime;

					reset_powerDef(&data_output_power);
					reset_powerDef(&total_out_driver_power);
	 				data_output = compute_dataoutput_data(parameters->cache_size,parameters->block_size,1,parameters->fully_assoc,Ndbl,Nspd,Ndwl,inrisetime,&outrisetime, &data_output_power);
	 				scale_powerDef(&data_output_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

	 				inrisetime = outrisetime;
	 				max_delay=MAX(max_delay, data_output);

        			subbank_v=0;
        			subbank_h=0;

        			subbank_routing_length(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,Ntbl,Ntwl,Ntspd,*NSubbanks,&subbank_v,&subbank_h) ;

        			if(*NSubbanks > 2 )
        			{
        				total_out_driver = compute_total_dataout_data(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,Ntbl,Ntspd,inrisetime,&outrisetime, subbank_v, subbank_h, &total_out_driver_power);
        			}

					/*
	    			    total_out_driver = total_out_driver_delay(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,Ntbl,Ntwl,Ntspd,*NSubbanks,inrisetime,&outrisetime, &total_out_driver_power);
					*/
        			scale_powerDef(&total_out_driver_power,(parameters->num_readwrite_ports+parameters->num_read_ports));
        			inrisetime = outrisetime;
        			max_delay=MAX(max_delay, total_out_driver);

					access_time=subbankaddress_routing_delay+ decoder_data+wordline_data+bitline_data+sense_amp_data+data_output+total_out_driver;

	 				/*
	 				 * Calcuate the cycle time
	 				 */

	 				//	 precharge_del = precharge_delay(wordline_data);

	 				//	 cycle_time = access_time + precharge_del;

	 				cycle_time=access_time/WAVE_PIPE;
	 				if (max_delay>cycle_time)  cycle_time=max_delay;

	 				/*
	 				 * The parameters are for a 0.8um process.  A quick way to
	 				 * scale the results to another process is to divide all
	 				 * the results by FUDGEFACTOR.  Normally, FUDGEFACTOR is 1.
	 				 */

	 				// changed by MnM
	 				// darray_senseamp_power+=(data_output + total_out_driver)*500e-6*5;

	 				sense_amp_data+=(data_output + total_out_driver)*500e-6*5;

	 				total_power.readOp.dynamic =(subbankaddress_routing_power.readOp.dynamic+darray_decoder_power.readOp.dynamic+darray_wordline_power.readOp.dynamic+darray_bitline_power.readOp.dynamic+darray_senseamp_power.readOp.dynamic+data_output_power.readOp.dynamic+total_out_driver_power.readOp.dynamic+darray_readControl_power.readOp.dynamic+darray_writeControl_power.readOp.dynamic)/FUDGEFACTOR;
	 				total_power.readOp.leakage =(subbankaddress_routing_power.readOp.leakage+darray_decoder_power.readOp.leakage+darray_wordline_power.readOp.leakage+darray_bitline_power.readOp.leakage+darray_senseamp_power.readOp.leakage+data_output_power.readOp.leakage+total_out_driver_power.readOp.leakage+darray_readControl_power.readOp.leakage+darray_writeControl_power.readOp.leakage);

     				total_power_without_routing.readOp.dynamic=(*NSubbanks)*(darray_decoder_power.readOp.dynamic+darray_wordline_power.readOp.dynamic+darray_bitline_power.readOp.dynamic+darray_senseamp_power.readOp.dynamic+data_output_power.readOp.dynamic+darray_readControl_power.readOp.dynamic+darray_writeControl_power.readOp.dynamic)/FUDGEFACTOR;
     				total_power_without_routing.readOp.leakage=(*NSubbanks)*(darray_decoder_power.readOp.leakage+darray_wordline_power.readOp.leakage+darray_bitline_power.readOp.leakage+darray_senseamp_power.readOp.leakage+data_output_power.readOp.leakage+darray_readControl_power.readOp.leakage+darray_writeControl_power.readOp.leakage);

        			total_power_allbanks.readOp.dynamic= total_power_without_routing.readOp.dynamic + total_routing_power.readOp.dynamic/FUDGEFACTOR;
					total_power_allbanks.readOp.leakage= total_power_without_routing.readOp.leakage + total_routing_power.readOp.leakage;

					total_power.writeOp.dynamic =(subbankaddress_routing_power.writeOp.dynamic+darray_decoder_power.writeOp.dynamic+darray_wordline_power.writeOp.dynamic+darray_bitline_power.writeOp.dynamic+darray_senseamp_power.writeOp.dynamic+data_output_power.writeOp.dynamic+total_out_driver_power.writeOp.dynamic+darray_readControl_power.writeOp.dynamic+darray_writeControl_power.writeOp.dynamic)/FUDGEFACTOR;
					total_power.writeOp.leakage =(subbankaddress_routing_power.writeOp.leakage+darray_decoder_power.writeOp.leakage+darray_wordline_power.writeOp.leakage+darray_bitline_power.writeOp.leakage+darray_senseamp_power.writeOp.leakage+data_output_power.writeOp.leakage+total_out_driver_power.writeOp.leakage+darray_readControl_power.writeOp.leakage+darray_writeControl_power.writeOp.leakage);

					total_power_without_routing.writeOp.dynamic=(*NSubbanks)*(darray_decoder_power.writeOp.dynamic+darray_wordline_power.writeOp.dynamic+darray_bitline_power.writeOp.dynamic+darray_senseamp_power.writeOp.dynamic+data_output_power.writeOp.dynamic+darray_readControl_power.writeOp.dynamic+darray_writeControl_power.writeOp.dynamic)/FUDGEFACTOR;
					total_power_without_routing.writeOp.leakage=(*NSubbanks)*(darray_decoder_power.writeOp.leakage+darray_wordline_power.writeOp.leakage+darray_bitline_power.writeOp.leakage+darray_senseamp_power.writeOp.leakage+data_output_power.writeOp.leakage+darray_readControl_power.writeOp.leakage+darray_writeControl_power.writeOp.leakage);

					total_power_allbanks.writeOp.dynamic= total_power_without_routing.writeOp.dynamic + total_routing_power.writeOp.dynamic/FUDGEFACTOR;
					total_power_allbanks.writeOp.leakage= total_power_without_routing.writeOp.leakage + total_routing_power.writeOp.leakage;

	 				if (counter==1)
	 				{
        				// if ((result->total_power/result->max_power)/2+(result->access_time/result->max_access_time) > ((total_power/result->max_power)/2+access_time/(result->max_access_time*FUDGEFACTOR)))
						// if ((result->total_power/result->max_power)/2+(result->access_time/result->max_access_time) + (min_efficiency/arearesult->efficiency)/4 + (arearesult->aspect_ratio_total/max_aspect_ratio_total)/3 > ((total_power/result->max_power)/2+access_time/(result->max_access_time*FUDGEFACTOR)+ (min_efficiency/efficiency)/4 + (arearesult_temp.aspect_ratio_total/max_aspect_ratio_total)/3))
	    				//	if (result->cycle_time+1e-11*(result->best_Ndwl+result->best_Ndbl+result->best_Nspd+result->best_Ntwl+result->best_Ntbl+result->best_Ntspd) > cycle_time/FUDGEFACTOR+1e-11*(Ndwl+Ndbl+Nspd+Ntwl+Ntbl+Ntspd))
        				if (( (result->total_power.readOp.dynamic + result->total_power.readOp.leakage)/(result->max_power.readOp.dynamic + result->max_power.readOp.leakage) )/2+(result->access_time/result->max_access_time) + (1.0/arearesult->efficiency)*10 + (arearesult->aspect_ratio_total/max_aspect_ratio_total)/(4*10) > (( (total_power.readOp.dynamic + total_power.readOp.leakage)/(result->max_power.readOp.dynamic + result->max_power.readOp.leakage) )/2 +access_time/(result->max_access_time*FUDGEFACTOR) + (1.0/efficiency)*10 + (arearesult_temp.aspect_ratio_total/max_aspect_ratio_total)/(4*10) ))
        				{

	    			   		result->senseext_scale = senseext_scale;
	    			   		copy_powerDef(&result->total_power,total_power);
        			    	copy_powerDef(&result->total_power_without_routing,total_power_without_routing);
        			    	copy_powerDef(&result->total_power_allbanks,total_power_allbanks);
        			    	copy_scaled_powerDef(&result->total_routing_power,total_routing_power,FUDGEFACTOR);
        			    	copy_scaled_powerDef(&result->subbank_address_routing_power,subbankaddress_routing_power,FUDGEFACTOR);

        			    	result->subbank_address_routing_delay = subbankaddress_routing_delay/FUDGEFACTOR;
        			    	result->cycle_time = cycle_time/FUDGEFACTOR;
	    			   		result->access_time = access_time/FUDGEFACTOR;
	    			   		result->best_Ndwl = Ndwl;
	    			   		result->best_Ndbl = Ndbl;
	    			   		result->best_Nspd = Nspd;
	    			   		result->best_Ntwl = Ntwl;
	    			   		result->best_Ntbl = Ntbl;
	    			   		result->best_Ntspd = Ntspd;
	    			   		result->decoder_delay_data = decoder_data/FUDGEFACTOR;
	    			   		result->decoder_delay_tag = decoder_tag/FUDGEFACTOR;
	    			   		result->dec_tag_driver = decoder_tag_driver/FUDGEFACTOR;
	    			   		result->dec_tag_3to8 = decoder_tag_3to8/FUDGEFACTOR;
	    			   		result->dec_tag_inv = decoder_tag_inv/FUDGEFACTOR;
	    			   		result->dec_data_driver = decoder_data_driver/FUDGEFACTOR;
	    			   		result->dec_data_3to8 = decoder_data_3to8/FUDGEFACTOR;
	       					result->dec_data_inv = decoder_data_inv/FUDGEFACTOR;
	    			   		result->wordline_delay_data = wordline_data/FUDGEFACTOR;
	    			   		result->wordline_delay_tag = wordline_tag/FUDGEFACTOR;
	    			   		result->bitline_delay_data = bitline_data/FUDGEFACTOR;
	    			   		result->bitline_delay_tag = bitline_tag/FUDGEFACTOR;
	    			   		result->sense_amp_delay_tag = sense_amp_tag/FUDGEFACTOR;
	    			   		result->sense_amp_delay_data = sense_amp_data/FUDGEFACTOR;
	    			   		result->total_out_driver_delay_data = total_out_driver/FUDGEFACTOR;
	    			   		result->compare_part_delay = compare_tag/FUDGEFACTOR;
	    			   		result->drive_mux_delay = mux_driver/FUDGEFACTOR;
	    			   		result->selb_delay = selb/FUDGEFACTOR;
	    			   		result->drive_valid_delay = valid_driver/FUDGEFACTOR;
	    			   		result->data_output_delay = data_output/FUDGEFACTOR;

	    			   		copy_scaled_powerDef(&result->decoder_power_data,darray_decoder_power,FUDGEFACTOR);
	    			   		copy_scaled_powerDef(&result->wordline_power_data,darray_wordline_power,FUDGEFACTOR);
	    			   		copy_scaled_powerDef(&result->bitline_power_data,darray_bitline_power,FUDGEFACTOR);
	    			   		copy_scaled_powerDef(&result->sense_amp_power_data,darray_senseamp_power,FUDGEFACTOR);
	    			   		copy_scaled_powerDef(&result->total_out_driver_power_data,total_out_driver_power,FUDGEFACTOR);
	    			   		copy_scaled_powerDef(&result->data_output_power,data_output_power,FUDGEFACTOR);

							copy_scaled_powerDef(&result->read_control_power_data, darray_readControl_power, FUDGEFACTOR);
		  					copy_scaled_powerDef(&result->write_control_power_data, darray_writeControl_power, FUDGEFACTOR);

							result->precharge_delay = precharge_del/FUDGEFACTOR;
		   					result->data_nor_inputs = data_nor_inputs;
	    			   		result->tag_nor_inputs = tag_nor_inputs;
	        		   		area_subbanked(ADDRESS_BITS,BITOUT,parameters->num_readwrite_ports,parameters->num_read_ports,parameters->num_write_ports,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,parameters,arearesult_subbanked,arearesult);
        			    	arearesult->efficiency = (*NSubbanks)*(area_all_dataramcells+area_all_tagramcells)*100/(calculate_area(*arearesult_subbanked,parameters->fudgefactor)/100000000.0);
        			    	arearesult->aspect_ratio_total = (arearesult_subbanked->height/arearesult_subbanked->width);
        			    	arearesult->aspect_ratio_total = (arearesult->aspect_ratio_total > 1.0) ? (arearesult->aspect_ratio_total) : 1.0/(arearesult->aspect_ratio_total) ;
        			    	arearesult->max_efficiency = max_efficiency;
     			   	    	arearesult->max_aspect_ratio_total = max_aspect_ratio_total;

						}
	 				}
	 				else
	 				{

	 					if (result->max_access_time < access_time/FUDGEFACTOR)
	 				   	result->max_access_time = access_time/FUDGEFACTOR;
	 					if ( (result->max_power.readOp.dynamic + result->max_power.readOp.leakage) < (total_power.readOp.dynamic + total_power.readOp.leakage))
	 					{
	 						copy_powerDef(&result->max_power,total_power);
						}
        				if (arearesult_temp.max_efficiency < efficiency)
        				{
        				    arearesult_temp.max_efficiency = efficiency;
							max_efficiency = efficiency;
						}
        				if (min_efficiency > efficiency)
        				{
        				    min_efficiency = efficiency;
        				}
 		       			if (max_aspect_ratio_total < aspect_ratio_total_temp)
 		       			    max_aspect_ratio_total = aspect_ratio_total_temp;
			   		}
				}
 		   	}
 		}
 	}

	if (verbose) {
		printf("Area efficient configuration: (");
		for (i=0; i<5; i++)
			printf("%d, ", aeConfig[i] );
		printf("%d) \n  Area = %f, Time = %f ns, Power (ReadOp(dyn,lkg), WriteOp(dyn,lkg)) = %4.2f (%6.5f %6.5f) mW; %4.2f (%6.5f %6.5f) mW;  \n\n", aeConfig[5], aeArea, aeTime*1e9, (aePower.readOp.leakage+aePower.readOp.dynamic)*1e3, aePower.readOp.dynamic*1e3, aePower.readOp.leakage*1e3, (aePower.writeOp.leakage+aePower.writeOp.dynamic)*1e3, aePower.writeOp.dynamic*1e3, aePower.writeOp.leakage*1e3);

		printf("Optimal time configuration: (");
			for (i=0; i<5; i++)
				printf("%d, ", hsConfig[i]);
		printf("%d) \n  Area = %f, Time = %f ns, Power (ReadOp(dyn,lkg), WriteOp(dyn,lkg)) = %4.2f (%6.5f %6.5f) mW; %4.2f (%6.5f %6.5f) mW; \n\n", hsConfig[5], hsArea, hsTime*1e9, (hsPower.readOp.leakage+hsPower.readOp.dynamic)*1e3, hsPower.readOp.dynamic*1e3, hsPower.readOp.leakage*1e3, (hsPower.writeOp.leakage+hsPower.writeOp.dynamic)*1e3, hsPower.writeOp.dynamic*1e3, hsPower.writeOp.leakage*1e3);

		printf("Lowest power configuration: (");
		for (i=0; i<5; i++)
			printf("%d, ", lpConfig[i]);
		printf("%d) \n  Area = %f, Time = %f ns, Power (ReadOp(dyn,lkg), WriteOp(dyn,lkg)) = %4.2f (%6.5f %6.5f) mW; %4.2f (%6.5f %6.5f) mW; \n\n", lpConfig[5], lpArea, lpTime*1e9, (lpPower.readOp.leakage+lpPower.readOp.dynamic)*1e3, lpPower.readOp.dynamic*1e3, lpPower.readOp.leakage*1e3, (lpPower.writeOp.leakage+lpPower.writeOp.dynamic)*1e3, lpPower.writeOp.dynamic*1e3, lpPower.writeOp.leakage*1e3);
	}
}

void evaluate_config(result_type *result,arearesult_type *arearesult,area_type *arearesult_subbanked,parameter_type *parameters,double *NSubbanks,int *configArgs)
{
	extern int organizational_parameters_valid(int B, int A, int C, int Ndwl, int Ndbl, int Nspd,
				int Ntwl, int Ntbl, int Ntspd, char assoc);
	extern void area_subbanked (int baddr, int b0, int RWP, int ERP, int EWP, int Ndbl, int Ndwl,
				int Nspd, int Ntbl, int Ntwl, int Ntspd, double NSubbanks,
				parameter_type *parameters, area_type *result_subbanked, arearesult_type *result);

	void reset_device_widths();

	void reset_powerDef(powerDef *power);
	void scale_powerDef(powerDef *power, int val);
	void copy_powerDef(powerDef *dest, powerDef source);
	void copy_scaled_powerDef(powerDef *dest, powerDef source, double val);
	void add_powerDef_vals(powerDef *sum, powerDef a, powerDef b);


   	arearesult_type arearesult_temp;
   	area_type arearesult_subbanked_temp;

   	int Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,inter_subbanks,rows,columns,cols_subarray,rows_subarray,tag_driver_size1,tag_driver_size2;
   	int l_muxdrv_v, l_muxdrv_h, exp, htree_int;
   	double Subbank_Efficiency, Total_Efficiency, max_efficiency, efficiency, min_efficiency;
   	double max_aspect_ratio_total ,aspect_ratio_total_temp;
   	double htree;
   	double bank_h, bank_v, subbank_h, subbank_v, inter_h, inter_v;
   	double wirelength_v, wirelength_h, wirelength_sense_v, wirelength_sense_h;
   	double access_time=0;
   	//double total_power=0;
   	powerDef total_power;
   	double before_mux=0.0,after_mux=0;
   	powerDef total_power_allbanks;
   	// double total_address_routing_power=0.0, total_power_without_routing=0.0;
   	powerDef total_routing_power, total_power_without_routing;
   	double subbankaddress_routing_delay=0.0;
   	powerDef subbankaddress_routing_power;
   	double decoder_data_driver=0.0,decoder_data_3to8=0.0,decoder_data_inv=0.0;
   	double decoder_data=0.0,decoder_tag=0.0,wordline_data=0.0,wordline_tag=0.0;
   	// double darray_decoder_power=0.0,tarray_decoder_power=0.0,darray_wordline_power=0.0,tarray_wordline_power=0.0;
   	powerDef darray_decoder_power,tarray_decoder_power,darray_wordline_power,tarray_wordline_power;
   	double decoder_tag_driver=0.0,decoder_tag_3to8=0.0,decoder_tag_inv=0.0;
   	powerDef darray_readControl_power, tarray_readControl_power;
   	powerDef darray_writeControl_power, tarray_writeControl_power;
   	double bitline_data=0.0,bitline_tag=0.0,sense_amp_data=0.0,sense_amp_tag=0.0;
   	// double darray_bitline_power=0.0,tarray_bitline_power=0.0,darray_senseamp_power=0.0,tarray_senseamp_power=0.0;
   	powerDef darray_bitline_power,tarray_bitline_power,darray_senseamp_power,tarray_senseamp_power;
   	double compare_tag=0.0,mux_driver=0.0,data_output=0.0,selb=0.0;
   	//double compare_tag_power=0.0, selb_power=0.0, mux_driver_power=0.0, valid_driver_power=0.0;
   	powerDef compare_tag_power, selb_power, mux_driver_power, valid_driver_power;
   	// double data_output_power=0.0;
   	powerDef data_output_power;
   	double time_till_compare=0.0,time_till_select=0.0,driver_cap=0.0,valid_driver=0.0;
   	double cycle_time=0.0, precharge_del=0.0;
   	double outrisetime=0.0,inrisetime=0.0,addr_inrisetime=0.0;
   	double senseext_scale=1.0;
   	double total_out_driver=0.0;
   	// double total_out_driver_power=0.0;
   	powerDef total_out_driver_power, datain_driver_power;
   	float scale_init;
   	int data_nor_inputs=1, tag_nor_inputs=1;
   	double tag_delay=0.0, tag_delay_part1=0.0, tag_delay_part2=0.0, tag_delay_part3=0.0, tag_delay_part4=0.0,tag_delay_part5=0.0,tag_delay_part6=0.0;
   	double max_delay=0;
   	int i, hsConfig[6], lpConfig[6], aeConfig[6];
   	double hsTime, hsArea, lpTime, lpArea, aeArea, aeTime;
   	powerDef hsPower, lpPower, aePower;
   	int counter;
   	double Tpre,Tpre_tag;
   	double tarray_read_power, tarray_write_power, darray_read_power, darray_write_power;

   	rows = parameters->number_of_sets;
   	columns = 8*parameters->block_size*parameters->associativity;
   	FUDGEFACTOR=parameters->fudgefactor;

   	// VddPow=4.5/(pow(FUDGEFACTOR,(2.0/3.0)));
   	VddPow = Vdd_init;

   	if (VddPow < 0.7) VddPow=0.7;

   	if (VddPow > 5.0) VddPow=5.0;

   	VbitprePow=VddPow*3.3/4.5;
   	parameters->VddPow=VddPow;

   	/* go through possible Ndbl,Ndwl and find the smallest */
   	/* Because of area considerations, I don't think it makes sense
   	   to break either dimension up larger than MAXN */

   	/* only try moving output drivers for set associative cache */
   	if (parameters->associativity!=1)
   		scale_init=0.1;
   	else
   		scale_init=1.0;

   	if (parameters->fully_assoc)
   	/* If model is a fully associative cache - use larger cell size */
    {
    	FACbitmetal=((32+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1)+parameters->num_read_ports))*Cmetal);
     	FACwordmetal=((8+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1))+WIREPITCH*(parameters->num_read_ports))*Cmetal);
       	FARbitmetal=(((32+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1)+parameters->num_read_ports))/WIREWIDTH)*Rmetal);
       	FARwordmetal=(((8+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1))+WIREPITCH*(parameters->num_read_ports))/WIREWIDTH)*Rmetal);
    }

   	Cbitmetal=((16+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1)+parameters->num_read_ports))*Cmetal);
   	Cwordmetal=((8+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1))+WIREPITCH*(parameters->num_read_ports))*Cmetal);
   	Rbitmetal=(((16+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1)+parameters->num_read_ports))/WIREWIDTH)*Rmetal);
   	Rwordmetal=(((8+2*WIREPITCH*((parameters->num_write_ports+parameters->num_readwrite_ports-1))+WIREPITCH*(parameters->num_read_ports))/WIREWIDTH)*Rmetal);

	Ndwl = configArgs[0];
	Nspd = configArgs[1];
	Ndbl = configArgs[2];
	Ntwl = configArgs[3];
	Ntspd = configArgs[4];
	Ntbl = configArgs[5];

	if (!parameters->fully_assoc) {
 		if (!organizational_parameters_valid
	 		(parameters->block_size, parameters->associativity, parameters->cache_size,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,parameters->fully_assoc)) {
				fprintf(stderr,"INVALID PARAMETERS SPECIFIED.. \n");
				exit(0);
		}


		bank_h=0;
		bank_v=0;

		if (8*parameters->block_size/BITOUT == 1 && Ndbl*Nspd==1) {
			muxover=1;
		}
		else {
		    if (Ndbl*Nspd > MAX_COL_MUX) {
		    	muxover=8*parameters->block_size/BITOUT;
		    }
		    else {
		    	if (8*parameters->block_size*Ndbl*Nspd/BITOUT > MAX_COL_MUX) {
		        	muxover=(8*parameters->block_size/BITOUT)/(MAX_COL_MUX/(Ndbl*Nspd));
		        }
		        else  {
		        	muxover=1;
		        }
			}
		}

		area_subbanked(ADDRESS_BITS,BITOUT,parameters->num_readwrite_ports,parameters->num_read_ports,parameters->num_write_ports,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,parameters,&arearesult_subbanked_temp,&arearesult_temp);
		Subbank_Efficiency= (area_all_dataramcells+area_all_tagramcells)*100/(arearesult_temp.totalarea/100000000.0);
		Total_Efficiency= (*NSubbanks)*(area_all_dataramcells+area_all_tagramcells)*100/(calculate_area(arearesult_subbanked_temp,parameters->fudgefactor)/100000000.0);

		//efficiency = Subbank_Efficiency;
		efficiency = Total_Efficiency;
		arearesult_temp.efficiency = efficiency;
		aspect_ratio_total_temp = (arearesult_subbanked_temp.height/arearesult_subbanked_temp.width);
		aspect_ratio_total_temp = (aspect_ratio_total_temp > 1.0) ? (aspect_ratio_total_temp) : 1.0/(aspect_ratio_total_temp) ;
		arearesult_temp.aspect_ratio_total = aspect_ratio_total_temp;

		reset_device_widths();
		compute_device_widths(parameters->cache_size, parameters->block_size, parameters->associativity,parameters->fully_assoc,Ndwl,Ndbl,Nspd);
		compute_tag_device_widths(parameters->cache_size, parameters->block_size, parameters->associativity,
								Ndwl,Ndbl,Nspd,Ntspd,Ntwl,Ntbl,(*NSubbanks));

		subbank_dim(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,&bank_h,&bank_v);
		//subbanks_routing_power(parameters->fully_assoc,parameters->associativity,*NSubbanks,&bank_h,&bank_v,&total_address_routing_power);
		//total_address_routing_power.dynamic *=(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports);
		//total_address_routing_power.leakage *=(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports);

		reset_powerDef(&subbankaddress_routing_power);
		reset_powerDef(&darray_decoder_power);
		reset_powerDef(&darray_wordline_power);
		reset_powerDef(&darray_bitline_power);
		reset_powerDef(&data_output_power);
		reset_powerDef(&darray_senseamp_power);
		reset_powerDef(&darray_readControl_power);
		reset_powerDef(&darray_writeControl_power);
		reset_powerDef(&total_out_driver_power);
		reset_powerDef(&total_out_driver_power);

		outrisetime = 0.0;

		if(*NSubbanks > 2 ) {
			subbankaddress_routing_delay= compute_address_routing_data(parameters->cache_size, parameters->block_size, parameters->associativity, parameters->fully_assoc, Ndwl, Ndbl, Nspd, Ntwl, Ntbl,Ntspd,*NSubbanks,&outrisetime,&subbankaddress_routing_power);
		}

		scale_powerDef(&subbankaddress_routing_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

		/* Calculate data side of cache */
		Tpre = 0;
		inrisetime = outrisetime;
		addr_inrisetime=outrisetime;
		max_delay=0;

		decoder_data = compute_decoder_data(parameters->cache_size,parameters->block_size,
								parameters->associativity,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd, *NSubbanks,
							 	&decoder_data_driver,&decoder_data_3to8,
							 	&decoder_data_inv,inrisetime,&outrisetime, &data_nor_inputs,
							 	&darray_decoder_power);
		scale_powerDef(&darray_decoder_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

		Tpre = decoder_data;
		max_delay=MAX(max_delay, decoder_data);
		inrisetime = outrisetime;

		wordline_data = compute_wordline_data(parameters->cache_size, parameters->block_size,
										parameters->associativity,Ndwl,Ndbl,Nspd,
				   						inrisetime,&outrisetime,&darray_wordline_power);
		scale_powerDef(&darray_wordline_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

		Tpre += wordline_data;
		inrisetime = outrisetime;
		max_delay=MAX(max_delay, wordline_data);


		bitline_data = compute_bitline_data(parameters->cache_size,parameters->associativity,
										parameters->block_size,Ndwl,Ndbl,Nspd,
				 						inrisetime,&outrisetime, &darray_bitline_power, Tpre);
		scale_powerDef(&darray_bitline_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

		inrisetime = outrisetime;
		max_delay=MAX(max_delay, bitline_data);

		//int temp_row;
		compute_read_control_data(parameters->cache_size,parameters->associativity,
										parameters->block_size,Ndwl,Ndbl,Nspd, *NSubbanks,&darray_readControl_power);
		compute_write_control_data(parameters->cache_size,parameters->associativity,
										parameters->block_size,Ndwl,Ndbl,Nspd, *NSubbanks,&darray_writeControl_power);
		//temp_row = parameters->cache_size/(parameters->block_size*parameters->associativity*Ndbl*Nspd);
		sense_amp_data = compute_sense_amp_data(parameters->cache_size,parameters->associativity,
											parameters->block_size,Ndwl,Ndbl,Nspd,inrisetime,&outrisetime, &darray_senseamp_power);

		scale_powerDef(&darray_senseamp_power, (parameters->num_readwrite_ports+parameters->num_read_ports));
		scale_powerDef(&darray_readControl_power, (parameters->num_readwrite_ports+parameters->num_read_ports));
		scale_powerDef(&darray_writeControl_power, (parameters->num_readwrite_ports+parameters->num_read_ports));
		max_delay=MAX(max_delay, sense_amp_data);

		inrisetime = outrisetime;

		data_output = compute_dataoutput_data(parameters->cache_size,parameters->block_size,
										parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,
				   						inrisetime,&outrisetime, &data_output_power);
		scale_powerDef(&data_output_power,(parameters->num_readwrite_ports+parameters->num_read_ports));
		max_delay=MAX(max_delay, data_output);
		inrisetime = outrisetime;

		compute_datain_power(parameters->cache_size,parameters->block_size,
							parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,
							&datain_driver_power);
	    scale_powerDef(&datain_driver_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

		subbank_v=0;
		subbank_h=0;


		subbank_routing_length(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,Ntbl,Ntwl,Ntspd,*NSubbanks,&subbank_v,&subbank_h) ;


		if(*NSubbanks > 2 ) {
			total_out_driver = compute_total_dataout_data(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,Ntbl,Ntspd,inrisetime,&outrisetime, subbank_v, subbank_h, &total_out_driver_power);
		}

		/*
		total_out_driver = total_out_driver_delay(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,Ntbl,Ntwl,Ntspd,*NSubbanks,inrisetime,&outrisetime, &total_out_driver_power);
		*/
		scale_powerDef(&total_out_driver_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

		inrisetime = outrisetime;
		max_delay=MAX(max_delay, total_out_driver);

		add_powerDef_vals(&total_routing_power, total_out_driver_power, subbankaddress_routing_power);

		/* if the associativity is 1, the data output can come right
			after the sense amp.   Otherwise, it has to wait until
			the data access has been done. */

		if (parameters->associativity==1) {
			before_mux = subbankaddress_routing_delay +decoder_data + wordline_data + bitline_data +
								sense_amp_data + total_out_driver + data_output;
			after_mux = 0;
		} else {
			before_mux = subbankaddress_routing_delay+ decoder_data + wordline_data + bitline_data +
							sense_amp_data;
			after_mux = data_output + total_out_driver;
		}

		/*
		 * Now worry about the tag side.
		 */

		reset_powerDef(&tarray_decoder_power);
		reset_powerDef(&tarray_wordline_power);
		reset_powerDef(&tarray_bitline_power);
		reset_powerDef(&tarray_senseamp_power);
		reset_powerDef(&tarray_readControl_power);
		reset_powerDef(&tarray_writeControl_power);

		decoder_tag = compute_decoder_tag_data(parameters->cache_size,
							parameters->block_size,parameters->associativity,
							Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,*NSubbanks,
							&decoder_tag_driver,&decoder_tag_3to8,
							&decoder_tag_inv,addr_inrisetime,&outrisetime,&tag_nor_inputs, &tarray_decoder_power);
		max_delay=MAX(max_delay, decoder_tag);
		inrisetime = outrisetime;
		scale_powerDef(&tarray_decoder_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));
		Tpre_tag = decoder_tag;

		wordline_tag = compute_wordline_tag_data(parameters->cache_size,parameters->block_size,
										parameters->associativity,Ndwl, Nspd, Ntspd,Ntwl,Ntbl,*NSubbanks,
				      					inrisetime,&outrisetime, &tarray_wordline_power);
		max_delay=MAX(max_delay, wordline_tag);
		Tpre_tag += wordline_tag;
		inrisetime = outrisetime;
		scale_powerDef(&tarray_wordline_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

		bitline_tag = compute_bitline_tag_data(parameters->cache_size,parameters->associativity,
									    parameters->block_size,Ntwl,Ntbl,Ntspd,*NSubbanks,
									    inrisetime,&outrisetime, &tarray_bitline_power,Tpre_tag);
		max_delay=MAX(max_delay, bitline_tag);
		inrisetime = outrisetime;
		scale_powerDef(&tarray_bitline_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

		sense_amp_tag = compute_sense_amp_tag_data(parameters->cache_size,parameters->associativity,
										    parameters->block_size,Ntwl,Ntbl,Ntspd,*NSubbanks,
										    inrisetime,&outrisetime,&tarray_senseamp_power);
		scale_powerDef(&tarray_senseamp_power,(ADDRESS_BITS+2 - (int)logtwo((double)parameters->cache_size) + (int)logtwo((double)parameters->associativity)) );

		max_delay=MAX(max_delay, sense_amp_tag);
		inrisetime = outrisetime;
		scale_powerDef(&tarray_senseamp_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

		compute_tag_read_control_data(parameters->cache_size,parameters->associativity,
								parameters->block_size,Ntwl,Ntbl,Ntspd, *NSubbanks,&tarray_readControl_power);
		compute_tag_write_control_data(parameters->cache_size,parameters->associativity,
								parameters->block_size,Ntwl,Ntbl,Ntspd, *NSubbanks,&tarray_writeControl_power);

		/* split comparator - look at half the address bits only */
		reset_powerDef(&compare_tag_power);
		compare_tag = compute_half_compare_data(parameters->cache_size,parameters->associativity,
									    Ntbl,Ntspd,*NSubbanks,inrisetime,&outrisetime, &compare_tag_power);
		scale_powerDef(&compare_tag_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

		inrisetime = outrisetime;
		max_delay=MAX(max_delay, compare_tag);

		reset_powerDef(&valid_driver_power);
		reset_powerDef(&mux_driver_power);
		reset_powerDef(&selb_power);

		if (parameters->associativity == 1) {
			mux_driver = 0;
			valid_driver = compute_valid_driver_data(parameters->cache_size,parameters->block_size,
							parameters->associativity,parameters->fully_assoc,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,inrisetime,&valid_driver_power);
			scale_powerDef(&valid_driver_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

			max_delay=MAX(max_delay, valid_driver);
			time_till_compare = subbankaddress_routing_delay+ decoder_tag + wordline_tag + bitline_tag +
								sense_amp_tag;
			time_till_select = time_till_compare+ compare_tag + valid_driver;


			/*
			* From the above info, calculate the total access time
			*/

			access_time = MAX(before_mux+after_mux,time_till_select);
		} else {
			double wirelength;

			/* default scale is full wirelength */
		    cols_subarray = (8*parameters->block_size*parameters->associativity*Nspd/Ndwl);
			rows_subarray = (parameters->cache_size/(parameters->block_size*parameters->associativity*Ndbl*Nspd));

		 	if(Ndwl*Ndbl==1) {
		    	l_muxdrv_v= 0;
		        l_muxdrv_h= cols_subarray;
		    }
			if(Ndwl*Ndbl==2 || Ndwl*Ndbl==4) {
		   		l_muxdrv_v= 0;
		   		l_muxdrv_h= 2*cols_subarray;
		    }

		   	if(Ndwl*Ndbl>4) {
		   		htree=logtwo((double)(Ndwl*Ndbl));
		        htree_int = (int) htree;
		        if (htree_int % 2 ==0) {
		             exp = (htree_int/2-1);
		             l_muxdrv_v = (powers(2,exp)-1)*rows_subarray;
		             l_muxdrv_h = sqrt(Ndwl*Ndbl)*cols_subarray;
		        }
				else {
					exp = (htree_int+1)/2-1;
		            l_muxdrv_v = (powers(2,exp)-1)*rows_subarray;
		        	l_muxdrv_h = sqrt(Ndwl*Ndbl/2)*cols_subarray;
		    	}
		   	}

			wirelength_v=(l_muxdrv_v);
		    wirelength_h=(l_muxdrv_h);

			/* dualin mux driver - added for split comparator
			- inverter replaced by nand gate */

			mux_driver = compute_mux_driver_dualin_data(parameters->cache_size,
				parameters->block_size,	parameters->associativity,Ndbl,Nspd,Ndwl,Ntbl,Ntspd,
				inrisetime,&outrisetime,wirelength_v,wirelength_h,&mux_driver_power);

			max_delay=MAX(max_delay, mux_driver);
			scale_powerDef(&mux_driver_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

			selb = compute_selb_tag_path_data(inrisetime,&outrisetime,&selb_power);
			max_delay=MAX(max_delay, selb);
			scale_powerDef(&selb_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

			valid_driver = 0;
			time_till_compare = subbankaddress_routing_delay+decoder_tag + wordline_tag + bitline_tag +sense_amp_tag;

			time_till_select = time_till_compare+ compare_tag + mux_driver+ selb;
			access_time = MAX(before_mux,time_till_select) +after_mux;
		}

		/*
		* Calcuate the cycle time
		*/

		// precharge_del = precharge_delay(wordline_data);

		// cycle_time = access_time + precharge_del;

		cycle_time=access_time/WAVE_PIPE;
		if (max_delay>cycle_time)
			cycle_time=max_delay;

		/*
		* The parameters are for a 0.8um process.  A quick way to
		* scale the results to another process is to divide all
		* the results by FUDGEFACTOR.  Normally, FUDGEFACTOR is 1.
		*/

		// Changed by MnM // felt no need for this.. (doesnt make sense)
		// darray_senseamp_power+=(data_output + total_out_driver)*500e-6*5;
		// if (!parameters->fully_assoc)
		// 	tarray_senseamp_power+=(time_till_compare+ compare_tag + mux_driver+ selb +data_output + total_out_driver)*500e-6*5;

		tarray_read_power = (tarray_decoder_power.readOp.dynamic+tarray_wordline_power.readOp.dynamic+tarray_bitline_power.readOp.dynamic+tarray_senseamp_power.readOp.dynamic+compare_tag_power.readOp.dynamic+valid_driver_power.readOp.dynamic+mux_driver_power.readOp.dynamic+selb_power.readOp.dynamic+tarray_readControl_power.readOp.dynamic+tarray_writeControl_power.readOp.dynamic)/FUDGEFACTOR +
		        			tarray_decoder_power.readOp.leakage+tarray_wordline_power.readOp.leakage+tarray_bitline_power.readOp.leakage+tarray_senseamp_power.readOp.leakage+compare_tag_power.readOp.leakage+valid_driver_power.readOp.leakage+mux_driver_power.readOp.leakage+selb_power.readOp.leakage+tarray_readControl_power.readOp.leakage+tarray_writeControl_power.readOp.leakage;

		darray_read_power =	(subbankaddress_routing_power.readOp.dynamic+darray_decoder_power.readOp.dynamic+darray_wordline_power.readOp.dynamic+darray_bitline_power.readOp.dynamic+darray_senseamp_power.readOp.dynamic+total_out_driver_power.readOp.dynamic+data_output_power.readOp.dynamic+darray_readControl_power.readOp.dynamic+darray_writeControl_power.readOp.dynamic+datain_driver_power.readOp.dynamic)/FUDGEFACTOR+
							subbankaddress_routing_power.readOp.leakage+darray_decoder_power.readOp.leakage+darray_wordline_power.readOp.leakage+darray_bitline_power.readOp.leakage+darray_senseamp_power.readOp.leakage+total_out_driver_power.readOp.leakage+data_output_power.readOp.leakage+darray_readControl_power.readOp.leakage+darray_writeControl_power.readOp.leakage+datain_driver_power.readOp.leakage;

		tarray_write_power = (tarray_decoder_power.writeOp.dynamic+tarray_wordline_power.writeOp.dynamic+tarray_bitline_power.writeOp.dynamic+tarray_senseamp_power.writeOp.dynamic+compare_tag_power.writeOp.dynamic+valid_driver_power.writeOp.dynamic+mux_driver_power.writeOp.dynamic+selb_power.writeOp.dynamic+tarray_readControl_power.writeOp.dynamic+tarray_writeControl_power.writeOp.dynamic)/FUDGEFACTOR +
						    tarray_decoder_power.writeOp.leakage+tarray_wordline_power.writeOp.leakage+tarray_bitline_power.writeOp.leakage+tarray_senseamp_power.writeOp.leakage+compare_tag_power.writeOp.leakage+valid_driver_power.writeOp.leakage+mux_driver_power.writeOp.leakage+selb_power.writeOp.leakage+tarray_readControl_power.writeOp.leakage+tarray_writeControl_power.writeOp.leakage;

		darray_write_power = (subbankaddress_routing_power.writeOp.dynamic+darray_decoder_power.writeOp.dynamic+darray_wordline_power.writeOp.dynamic+darray_bitline_power.writeOp.dynamic+darray_senseamp_power.writeOp.dynamic+total_out_driver_power.writeOp.dynamic+data_output_power.writeOp.dynamic+darray_readControl_power.writeOp.dynamic+darray_writeControl_power.writeOp.dynamic+datain_driver_power.writeOp.dynamic)/FUDGEFACTOR+
							subbankaddress_routing_power.writeOp.leakage+darray_decoder_power.writeOp.leakage+darray_wordline_power.writeOp.leakage+darray_bitline_power.writeOp.leakage+darray_senseamp_power.writeOp.leakage+total_out_driver_power.writeOp.leakage+data_output_power.writeOp.leakage+darray_readControl_power.writeOp.leakage+darray_writeControl_power.writeOp.leakage+datain_driver_power.writeOp.leakage;

		total_power.readOp.dynamic =(subbankaddress_routing_power.readOp.dynamic+darray_decoder_power.readOp.dynamic+darray_wordline_power.readOp.dynamic+darray_bitline_power.readOp.dynamic+darray_senseamp_power.readOp.dynamic+total_out_driver_power.readOp.dynamic+tarray_decoder_power.readOp.dynamic+tarray_wordline_power.readOp.dynamic+tarray_bitline_power.readOp.dynamic+tarray_senseamp_power.readOp.dynamic+compare_tag_power.readOp.dynamic+valid_driver_power.readOp.dynamic+mux_driver_power.readOp.dynamic+selb_power.readOp.dynamic+data_output_power.readOp.dynamic+darray_readControl_power.readOp.dynamic+tarray_readControl_power.readOp.dynamic+darray_writeControl_power.readOp.dynamic+tarray_writeControl_power.readOp.dynamic+datain_driver_power.readOp.dynamic)/FUDGEFACTOR;
		total_power_without_routing.readOp.dynamic =(*NSubbanks)*(darray_decoder_power.readOp.dynamic+darray_wordline_power.readOp.dynamic+darray_bitline_power.readOp.dynamic+darray_senseamp_power.readOp.dynamic+tarray_decoder_power.readOp.dynamic+tarray_wordline_power.readOp.dynamic+tarray_bitline_power.readOp.dynamic+tarray_senseamp_power.readOp.dynamic+compare_tag_power.readOp.dynamic+mux_driver_power.readOp.dynamic+selb_power.readOp.dynamic+data_output_power.readOp.dynamic+darray_readControl_power.readOp.dynamic+tarray_readControl_power.readOp.dynamic+darray_writeControl_power.readOp.dynamic+tarray_writeControl_power.readOp.dynamic)/FUDGEFACTOR+valid_driver_power.readOp.dynamic/FUDGEFACTOR;
		total_power_allbanks.readOp.dynamic = total_power_without_routing.readOp.dynamic + total_routing_power.readOp.dynamic/FUDGEFACTOR;

		total_power.readOp.leakage =(subbankaddress_routing_power.readOp.leakage+darray_decoder_power.readOp.leakage+darray_wordline_power.readOp.leakage+darray_bitline_power.readOp.leakage+darray_senseamp_power.readOp.leakage+total_out_driver_power.readOp.leakage+tarray_decoder_power.readOp.leakage+tarray_wordline_power.readOp.leakage+tarray_bitline_power.readOp.leakage+tarray_senseamp_power.readOp.leakage+compare_tag_power.readOp.leakage+valid_driver_power.readOp.leakage+mux_driver_power.readOp.leakage+selb_power.readOp.leakage+data_output_power.readOp.leakage+darray_readControl_power.readOp.leakage+tarray_readControl_power.readOp.leakage+darray_writeControl_power.readOp.leakage+tarray_writeControl_power.readOp.leakage+datain_driver_power.readOp.leakage);
		total_power_without_routing.readOp.leakage =(*NSubbanks)*(darray_decoder_power.readOp.leakage+darray_wordline_power.readOp.leakage+darray_bitline_power.readOp.leakage+darray_senseamp_power.readOp.leakage+tarray_decoder_power.readOp.leakage+tarray_wordline_power.readOp.leakage+tarray_bitline_power.readOp.leakage+tarray_senseamp_power.readOp.leakage+compare_tag_power.readOp.leakage+mux_driver_power.readOp.leakage+selb_power.readOp.leakage+data_output_power.readOp.leakage+darray_readControl_power.readOp.leakage+tarray_readControl_power.readOp.leakage+darray_writeControl_power.readOp.leakage+tarray_writeControl_power.readOp.leakage)+valid_driver_power.readOp.leakage;
		total_power_allbanks.readOp.leakage = total_power_without_routing.readOp.leakage + total_routing_power.readOp.leakage;

		total_power.writeOp.dynamic =(subbankaddress_routing_power.writeOp.dynamic+darray_decoder_power.writeOp.dynamic+darray_wordline_power.writeOp.dynamic+darray_bitline_power.writeOp.dynamic+darray_senseamp_power.writeOp.dynamic+total_out_driver_power.writeOp.dynamic+data_output_power.writeOp.dynamic+darray_writeControl_power.writeOp.dynamic+datain_driver_power.writeOp.dynamic+darray_readControl_power.writeOp.dynamic+tarray_decoder_power.readOp.dynamic+tarray_wordline_power.readOp.dynamic+tarray_bitline_power.readOp.dynamic+tarray_senseamp_power.readOp.dynamic+compare_tag_power.readOp.dynamic+valid_driver_power.readOp.dynamic+mux_driver_power.readOp.dynamic+selb_power.readOp.dynamic+tarray_readControl_power.readOp.dynamic+tarray_writeControl_power.readOp.dynamic)/FUDGEFACTOR;
		total_power_without_routing.writeOp.dynamic =(*NSubbanks)*(darray_decoder_power.writeOp.dynamic+darray_wordline_power.writeOp.dynamic+darray_bitline_power.writeOp.dynamic+darray_senseamp_power.writeOp.dynamic+tarray_decoder_power.writeOp.dynamic+tarray_wordline_power.writeOp.dynamic+tarray_bitline_power.writeOp.dynamic+tarray_senseamp_power.writeOp.dynamic+compare_tag_power.writeOp.dynamic+mux_driver_power.writeOp.dynamic+selb_power.writeOp.dynamic+data_output_power.writeOp.dynamic+darray_readControl_power.writeOp.dynamic+tarray_readControl_power.writeOp.dynamic+darray_writeControl_power.writeOp.dynamic+tarray_writeControl_power.writeOp.dynamic)/FUDGEFACTOR+valid_driver_power.readOp.dynamic/FUDGEFACTOR;
		total_power_allbanks.writeOp.dynamic = total_power_without_routing.writeOp.dynamic + total_routing_power.writeOp.dynamic/FUDGEFACTOR;

		total_power.writeOp.leakage =(subbankaddress_routing_power.writeOp.leakage+darray_decoder_power.writeOp.leakage+darray_wordline_power.writeOp.leakage+darray_bitline_power.writeOp.leakage+darray_senseamp_power.writeOp.leakage+total_out_driver_power.writeOp.leakage+data_output_power.writeOp.leakage+darray_writeControl_power.writeOp.leakage+datain_driver_power.writeOp.leakage+darray_readControl_power.writeOp.leakage+tarray_decoder_power.readOp.leakage+tarray_wordline_power.readOp.leakage+tarray_bitline_power.readOp.leakage+tarray_senseamp_power.readOp.leakage+compare_tag_power.readOp.leakage+valid_driver_power.readOp.leakage+mux_driver_power.readOp.leakage+selb_power.readOp.leakage+tarray_readControl_power.readOp.leakage+tarray_writeControl_power.readOp.leakage)/FUDGEFACTOR;
		total_power_without_routing.writeOp.leakage =(*NSubbanks)*(darray_decoder_power.writeOp.leakage+darray_wordline_power.writeOp.leakage+darray_bitline_power.writeOp.leakage+darray_senseamp_power.writeOp.leakage+tarray_decoder_power.writeOp.leakage+tarray_wordline_power.writeOp.leakage+tarray_bitline_power.writeOp.leakage+tarray_senseamp_power.writeOp.leakage+compare_tag_power.writeOp.leakage+mux_driver_power.writeOp.leakage+selb_power.writeOp.leakage+data_output_power.writeOp.leakage+darray_readControl_power.writeOp.leakage+tarray_readControl_power.writeOp.leakage+darray_writeControl_power.writeOp.leakage+tarray_writeControl_power.writeOp.leakage)/FUDGEFACTOR+valid_driver_power.readOp.leakage/FUDGEFACTOR;
		total_power_allbanks.writeOp.leakage = total_power_without_routing.writeOp.leakage + total_routing_power.writeOp.leakage;

		result->senseext_scale = senseext_scale;
		copy_powerDef(&result->total_power, total_power);
		copy_powerDef(&result->total_power_without_routing, total_power_without_routing);
		copy_powerDef(&result->total_power_allbanks, total_power_allbanks);

		copy_scaled_powerDef(&result->total_routing_power, total_routing_power, FUDGEFACTOR);
		copy_scaled_powerDef(&result->subbank_address_routing_power, subbankaddress_routing_power, FUDGEFACTOR);

		//if (verbose == 1 ) {
			//fprintf(stderr, "(%d, %d, %d, %d, %d, %d) ",Ndwl, Nspd, Ndbl, Ntwl, Ntspd, Ntbl);
			//fprintf(stderr, "Pow - %f, Acc - %f, Pow - %f, Acc - %f, Combo - %f\n", (total_power.dynamic+total_power.leakage)*1e3, access_time*1e3, (total_power.dynamic+total_power.leakage)/(result->max_power.dynamic+result->max_power.leakage), access_time/result->max_access_time, (total_power.dynamic+total_power.leakage)*access_time/((result->max_power.dynamic+result->max_power.leakage)*result->max_access_time) );
	//		fprintf(stderr, " Cache read hit power = %5.4f mW \n Cache write hit power = %5.4f mW\n",(tarray_read_power+darray_read_power)*1e3, (tarray_read_power+darray_write_power)*1e3);
			// for a read miss, there wud be a tag read, data read, line fill (tag write and data write).
			// for a write miss, there wud be a tag read and line fill (tag write and data write)
	//    	fprintf(stderr, " Cache read miss power = %5.4f mW\n Cache write miss power = %5.4f mW\n",(tarray_read_power+darray_read_power+tarray_write_power+darray_write_power)*1e3,(tarray_read_power+tarray_write_power+darray_write_power)*1e3);
	//		fprintf(stderr, " CacheReadAccessHit Power Contributions: (%d, %d, %d, %d, %d, %d) %f mW %fns \n", Ndwl, Nspd, Ndbl, Ntwl, Ntspd, Ntbl, (total_power.readOp.dynamic+total_power.readOp.leakage)*1e3, access_time/FUDGEFACTOR*1e9);
	//		fprintf(stderr, "   (%5.4f, %5.4f) (%7.6f, %7.6f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f)\n     (%5.4f, %5.4f) (%7.6f, %7.6f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f)\n     (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f)\n     (%7.6f %7.6f) (%7.6f %7.6f) (%7.6f %7.6f) (%5.4f, %5.4f) \n",

	//				darray_decoder_power.readOp.dynamic/FUDGEFACTOR*1e3, darray_decoder_power.readOp.leakage*1e3,
	//		    	darray_wordline_power.readOp.dynamic/FUDGEFACTOR*1e3, darray_wordline_power.readOp.leakage*1e3,
	//		    	darray_bitline_power.readOp.dynamic/FUDGEFACTOR*1e3, darray_bitline_power.readOp.leakage*1e3,
	//		    	darray_senseamp_power.readOp.dynamic/FUDGEFACTOR*1e3, darray_senseamp_power.readOp.leakage*1e3,
	//		    	darray_readControl_power.readOp.dynamic/FUDGEFACTOR*1e3, darray_readControl_power.readOp.leakage*1e3,
	//		    	darray_writeControl_power.readOp.dynamic/FUDGEFACTOR*1e3, darray_writeControl_power.readOp.leakage*1e3,

	//				tarray_decoder_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_decoder_power.readOp.leakage*1e3,
	//		    	tarray_wordline_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_wordline_power.readOp.leakage*1e3,
	//		    	tarray_bitline_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_bitline_power.readOp.leakage*1e3,
	//		    	tarray_senseamp_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_senseamp_power.readOp.leakage*1e3,
	//				tarray_readControl_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_readControl_power.readOp.leakage*1e3,
	//		    	tarray_writeControl_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_writeControl_power.readOp.leakage*1e3,
//
	//		    	compare_tag_power.readOp.dynamic/FUDGEFACTOR*1e3, compare_tag_power.readOp.leakage*1e3,
	//		    	valid_driver_power.readOp.dynamic/FUDGEFACTOR*1e3, valid_driver_power.readOp.leakage*1e3,
	//		    	mux_driver_power.readOp.dynamic/FUDGEFACTOR*1e3, mux_driver_power.readOp.leakage*1e3,
	//		    	selb_power.readOp.dynamic/FUDGEFACTOR*1e3, selb_power.readOp.leakage*1e3,
	//		    	subbankaddress_routing_power.readOp.dynamic/FUDGEFACTOR*1e3, subbankaddress_routing_power.readOp.leakage*1e3,
	//				total_out_driver_power.readOp.dynamic/FUDGEFACTOR*1e3, total_out_driver_power.readOp.leakage*1e3,
	//				datain_driver_power.readOp.dynamic/FUDGEFACTOR*1e3, datain_driver_power.readOp.leakage*1e3,
	//		    	data_output_power.readOp.dynamic/FUDGEFACTOR*1e3, data_output_power.readOp.leakage*1e3);
	//		 fprintf(stderr, "CacheWriteAccessHit Power Contributions: (%d, %d, %d, %d, %d, %d) %f mW %fns \n", Ndwl, Nspd, Ndbl, Ntwl, Ntspd, Ntbl, (total_power.writeOp.dynamic+total_power.writeOp.leakage)*1e3);
	//		 fprintf(stderr, " (%5.4f, %5.4f) (%7.6f, %7.6f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f)\n     (%5.4f, %5.4f) (%7.6f, %7.6f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f)\n     (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f) (%5.4f, %5.4f)\n     (%7.6f %7.6f) (%7.6f %7.6f) (%7.6f %7.6f) (%5.4f, %5.4f) \n",
//
	//    			darray_decoder_power.writeOp.dynamic/FUDGEFACTOR*1e3, darray_decoder_power.writeOp.leakage*1e3,
	//    			darray_wordline_power.writeOp.dynamic/FUDGEFACTOR*1e3, darray_wordline_power.writeOp.leakage*1e3,
	//    			darray_bitline_power.writeOp.dynamic/FUDGEFACTOR*1e3, darray_bitline_power.writeOp.leakage*1e3,
	//    			darray_senseamp_power.writeOp.dynamic/FUDGEFACTOR*1e3, darray_senseamp_power.writeOp.leakage*1e3,
	//    			darray_readControl_power.writeOp.dynamic/FUDGEFACTOR*1e3, darray_readControl_power.writeOp.leakage*1e3,
	//    			darray_writeControl_power.writeOp.dynamic/FUDGEFACTOR*1e3, darray_writeControl_power.writeOp.leakage*1e3,
////
	//    			tarray_decoder_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_decoder_power.readOp.leakage*1e3,
	//    			tarray_wordline_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_wordline_power.readOp.leakage*1e3,
	//    			tarray_bitline_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_bitline_power.readOp.leakage*1e3,
	//    			tarray_senseamp_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_senseamp_power.readOp.leakage*1e3,
	//    			tarray_readControl_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_readControl_power.readOp.leakage*1e3,
	//    			tarray_writeControl_power.readOp.dynamic/FUDGEFACTOR*1e3, tarray_writeControl_power.readOp.leakage*1e3,

	//    			compare_tag_power.readOp.dynamic/FUDGEFACTOR*1e3, compare_tag_power.readOp.leakage*1e3,
	//    			valid_driver_power.readOp.dynamic/FUDGEFACTOR*1e3, valid_driver_power.readOp.leakage*1e3,
	//    			mux_driver_power.readOp.dynamic/FUDGEFACTOR*1e3, mux_driver_power.readOp.leakage*1e3,
	//    			selb_power.readOp.dynamic/FUDGEFACTOR*1e3, selb_power.readOp.leakage*1e3,
	//    			subbankaddress_routing_power.writeOp.dynamic/FUDGEFACTOR*1e3, subbankaddress_routing_power.writeOp.leakage*1e3,
	//				total_out_driver_power.writeOp.dynamic/FUDGEFACTOR*1e3, total_out_driver_power.writeOp.leakage*1e3,
	//				datain_driver_power.writeOp.dynamic/FUDGEFACTOR*1e3, datain_driver_power.writeOp.leakage*1e3,
	//    			data_output_power.writeOp.dynamic/FUDGEFACTOR*1e3, data_output_power.writeOp.leakage*1e3);
	//    //}

		result->subbank_address_routing_delay = subbankaddress_routing_delay/FUDGEFACTOR;
		result->cycle_time = cycle_time/FUDGEFACTOR;
		result->access_time = access_time/FUDGEFACTOR;
		result->best_muxover = muxover;
		result->best_Ndwl = Ndwl;
		result->best_Ndbl = Ndbl;
		result->best_Nspd = Nspd;
		result->best_Ntwl = Ntwl;
		result->best_Ntbl = Ntbl;
		result->best_Ntspd = Ntspd;
		result->dec_tag_driver = decoder_tag_driver/FUDGEFACTOR;
		result->dec_tag_3to8 = decoder_tag_3to8/FUDGEFACTOR;
		result->dec_tag_inv = decoder_tag_inv/FUDGEFACTOR;
		result->dec_data_driver = decoder_data_driver/FUDGEFACTOR;
		result->dec_data_3to8 = decoder_data_3to8/FUDGEFACTOR;
		result->dec_data_inv = decoder_data_inv/FUDGEFACTOR;
		result->wordline_delay_data = wordline_data/FUDGEFACTOR;

		result->decoder_delay_data = decoder_data/FUDGEFACTOR;
		result->decoder_delay_tag = decoder_tag/FUDGEFACTOR;
		result->wordline_delay_tag = wordline_tag/FUDGEFACTOR;
		result->bitline_delay_data = bitline_data/FUDGEFACTOR;
		result->bitline_delay_tag = bitline_tag/FUDGEFACTOR;
		result->sense_amp_delay_data = sense_amp_data/FUDGEFACTOR;
		result->sense_amp_delay_tag = sense_amp_tag/FUDGEFACTOR;

		result->total_out_driver_delay_data = total_out_driver/FUDGEFACTOR;
		result->compare_part_delay = compare_tag/FUDGEFACTOR;
		result->drive_mux_delay = mux_driver/FUDGEFACTOR;
		result->selb_delay = selb/FUDGEFACTOR;
		result->drive_valid_delay = valid_driver/FUDGEFACTOR;
		result->data_output_delay = data_output/FUDGEFACTOR;
		result->precharge_delay = precharge_del/FUDGEFACTOR;

		copy_scaled_powerDef(&result->decoder_power_data,darray_decoder_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->decoder_power_tag,tarray_decoder_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->wordline_power_data,darray_wordline_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->wordline_power_tag,tarray_wordline_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->bitline_power_data,darray_bitline_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->bitline_power_tag,tarray_bitline_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->sense_amp_power_data,darray_senseamp_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->sense_amp_power_tag,tarray_senseamp_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->total_out_driver_power_data,total_out_driver_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->compare_part_power,compare_tag_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->drive_mux_power,mux_driver_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->selb_power,selb_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->drive_valid_power,valid_driver_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->data_output_power,data_output_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->read_control_power_data,darray_readControl_power, FUDGEFACTOR);
		copy_scaled_powerDef(&result->read_control_power_tag,tarray_readControl_power, FUDGEFACTOR);
		copy_scaled_powerDef(&result->write_control_power_data,darray_writeControl_power, FUDGEFACTOR);
		copy_scaled_powerDef(&result->write_control_power_tag,tarray_writeControl_power, FUDGEFACTOR);

		result->darray_read_power = darray_read_power;
		result->darray_write_power = darray_write_power;
		result->tarray_read_power = tarray_read_power;
		result->tarray_write_power = tarray_write_power;

		result->data_nor_inputs = data_nor_inputs;
		result->tag_nor_inputs = tag_nor_inputs;
		area_subbanked(ADDRESS_BITS,BITOUT,parameters->num_readwrite_ports,parameters->num_read_ports,parameters->num_write_ports,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,parameters,arearesult_subbanked,arearesult);
		arearesult->efficiency = (*NSubbanks)*(area_all_dataramcells+area_all_tagramcells)*100/(calculate_area(*arearesult_subbanked,parameters->fudgefactor)/100000000.0);
		arearesult->aspect_ratio_total = (arearesult_subbanked->height/arearesult_subbanked->width);
		arearesult->aspect_ratio_total = (arearesult->aspect_ratio_total > 1.0) ? (arearesult->aspect_ratio_total) : 1.0/(arearesult->aspect_ratio_total) ;
		arearesult->max_efficiency = max_efficiency;
		arearesult->max_aspect_ratio_total = max_aspect_ratio_total;
	}

	else	{
    	/* Fully associative model - only vary Ndbl|Ntbl */

		if (!organizational_parameters_valid
		   (parameters->block_size, 1, parameters->cache_size,Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,parameters->fully_assoc))
		{
			fprintf(stderr,"INVALID PARAMETERS SPECIFIED.. \n");
			exit(0);
		}

		if (8*parameters->block_size/BITOUT == 1 && Ndbl*Nspd==1)
		{
		   muxover=1;
		}
		else
		{
			if (Ndbl*Nspd > MAX_COL_MUX)
			{
				muxover=8*parameters->block_size/BITOUT;
			}
			else
			{
				if (8*parameters->block_size*Ndbl*Nspd/BITOUT > MAX_COL_MUX)
				{
					muxover=(8*parameters->block_size/BITOUT)/(MAX_COL_MUX/(Ndbl*Nspd));
				}
				else
				{
					muxover=1;
				}
			}
		}


		area_subbanked(ADDRESS_BITS,BITOUT,parameters->num_readwrite_ports,parameters->num_read_ports,parameters->num_write_ports,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,parameters,&arearesult_subbanked_temp,&arearesult_temp);

		Subbank_Efficiency= (area_all_dataramcells+area_all_tagramcells)*100/(arearesult_temp.totalarea/100000000.0);
		Total_Efficiency= (*NSubbanks)*(area_all_dataramcells+area_all_tagramcells)*100/(calculate_area(arearesult_subbanked_temp,parameters->fudgefactor)/100000000.0);
		// efficiency = Subbank_Efficiency;
		efficiency = Total_Efficiency;
		arearesult_temp.efficiency = efficiency;
		aspect_ratio_total_temp = (arearesult_subbanked_temp.height/arearesult_subbanked_temp.width);
		aspect_ratio_total_temp = (aspect_ratio_total_temp > 1.0) ? (aspect_ratio_total_temp) : 1.0/(aspect_ratio_total_temp) ;

		arearesult_temp.aspect_ratio_total = aspect_ratio_total_temp;

		//compute_device_widths(parameters->cache_size, parameters->block_size, parameters->associativity,
		//	  					Ndwl,Ndbl,Nspd);

		bank_h=0;
		bank_v=0;
		subbank_dim(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,&bank_h,&bank_v);
		//subbanks_routing_power(parameters->fully_assoc,parameters->associativity,*NSubbanks,&bank_h,&bank_v,&total_address_routing_power);
		//total_address_routing_power.dynamic *=(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports);
		//total_address_routing_power.leakage *=(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports);

		reset_powerDef(&subbankaddress_routing_power);
		reset_powerDef(&darray_decoder_power);

		if(*NSubbanks > 2 )
		{
			subbankaddress_routing_delay= compute_address_routing_data(parameters->cache_size, parameters->block_size, parameters->associativity, parameters->fully_assoc, Ndwl, Ndbl, Nspd, Ntwl, Ntbl,Ntspd,*NSubbanks,&outrisetime,&subbankaddress_routing_power);
		}

		scale_powerDef(&subbankaddress_routing_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

		/* Calculate data side of cache */
		inrisetime = outrisetime;
		addr_inrisetime=outrisetime;

		max_delay=0;

		/* tag path contained here */
		decoder_data = fa_tag_delay(parameters->cache_size,
							parameters->block_size, Ndwl,Ndbl,Nspd,Ntwl,Ntbl,Ntspd,
							&tag_delay_part1,&tag_delay_part2,
							&tag_delay_part3,&tag_delay_part4,
							&tag_delay_part5,&tag_delay_part6,
							&outrisetime,&tag_nor_inputs, &darray_decoder_power);
		scale_powerDef(&darray_decoder_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

		inrisetime = outrisetime;
		max_delay=MAX(max_delay, decoder_data);
		Tpre = decoder_data;

		reset_powerDef(&darray_wordline_power);
		wordline_data = compute_wordline_data(parameters->cache_size, parameters->block_size,1,Ndwl,Ndbl,Nspd,
								inrisetime,&outrisetime, &darray_wordline_power);
		scale_powerDef(&darray_wordline_power,(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

		inrisetime = outrisetime;
		max_delay=MAX(max_delay, wordline_data);
		Tpre += wordline_data;

		reset_powerDef(&darray_bitline_power);
		bitline_data = compute_bitline_data(parameters->cache_size,1,
					      parameters->block_size,Ndwl,Ndbl,Nspd,
					      inrisetime,&outrisetime,&darray_bitline_power, Tpre);
		scale_powerDef(&darray_bitline_power, (parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports));

		inrisetime = outrisetime;
		max_delay=MAX(max_delay, bitline_data);
		reset_powerDef(&darray_senseamp_power);
		reset_powerDef(&darray_readControl_power);
		reset_powerDef(&darray_writeControl_power);
		{
			//int temp_row;
			compute_read_control_data(parameters->cache_size,parameters->associativity,
								parameters->block_size,Ndwl,Ndbl,Nspd, *NSubbanks,&darray_readControl_power);
			compute_write_control_data(parameters->cache_size,parameters->associativity,
								parameters->block_size,Ndwl,Ndbl,Nspd, *NSubbanks,&darray_writeControl_power);
			//temp_row = parameters->cache_size/(parameters->block_size*Ndbl);

			sense_amp_data = compute_sense_amp_data(parameters->cache_size,parameters->associativity,
											parameters->block_size,Ndwl,Ndbl,Nspd,inrisetime,&outrisetime, &darray_senseamp_power);
			scale_powerDef(&darray_senseamp_power,BITOUT*muxover/2);
			scale_powerDef(&darray_senseamp_power,(parameters->num_readwrite_ports+parameters->num_read_ports));
			scale_powerDef(&darray_readControl_power,(parameters->num_readwrite_ports+parameters->num_read_ports));
			scale_powerDef(&darray_writeControl_power,(parameters->num_readwrite_ports+parameters->num_read_ports));
			max_delay=MAX(max_delay, sense_amp_data);
		}

		inrisetime = outrisetime;

		reset_powerDef(&data_output_power);
		reset_powerDef(&total_out_driver_power);
		data_output = compute_dataoutput_data(parameters->cache_size,parameters->block_size,1,parameters->fully_assoc,Ndbl,Nspd,Ndwl,inrisetime,&outrisetime, &data_output_power);
		scale_powerDef(&data_output_power,(parameters->num_readwrite_ports+parameters->num_read_ports));

		inrisetime = outrisetime;
		max_delay=MAX(max_delay, data_output);

		subbank_v=0;
		subbank_h=0;

		subbank_routing_length(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,Ntbl,Ntwl,Ntspd,*NSubbanks,&subbank_v,&subbank_h) ;

		if(*NSubbanks > 2 )
		{
			total_out_driver = compute_total_dataout_data(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,Ntbl,Ntspd,inrisetime,&outrisetime, subbank_v, subbank_h, &total_out_driver_power);
		}

		/*
			total_out_driver = total_out_driver_delay(parameters->cache_size,parameters->block_size,parameters->associativity,parameters->fully_assoc,Ndbl,Nspd,Ndwl,Ntbl,Ntwl,Ntspd,*NSubbanks,inrisetime,&outrisetime, &total_out_driver_power);
		*/
		scale_powerDef(&total_out_driver_power,(parameters->num_readwrite_ports+parameters->num_read_ports));
		inrisetime = outrisetime;
		max_delay=MAX(max_delay, total_out_driver);

		access_time=subbankaddress_routing_delay+ decoder_data+wordline_data+bitline_data+sense_amp_data+data_output+total_out_driver;

		/*
		 * Calcuate the cycle time
		 */

		//	 precharge_del = precharge_delay(wordline_data);

		//	 cycle_time = access_time + precharge_del;

		cycle_time=access_time/WAVE_PIPE;
		if (max_delay>cycle_time)  cycle_time=max_delay;

		/*
		 * The parameters are for a 0.8um process.  A quick way to
		 * scale the results to another process is to divide all
		 * the results by FUDGEFACTOR.  Normally, FUDGEFACTOR is 1.
		 */

		// changed by MnM
		// darray_senseamp_power+=(data_output + total_out_driver)*500e-6*5;

		sense_amp_data+=(data_output + total_out_driver)*500e-6*5;

		total_power.readOp.dynamic =(subbankaddress_routing_power.readOp.dynamic+darray_decoder_power.readOp.dynamic+darray_wordline_power.readOp.dynamic+darray_bitline_power.readOp.dynamic+darray_senseamp_power.readOp.dynamic+data_output_power.readOp.dynamic+total_out_driver_power.readOp.dynamic+darray_readControl_power.readOp.dynamic+darray_writeControl_power.readOp.dynamic)/FUDGEFACTOR;
		total_power.readOp.leakage =(subbankaddress_routing_power.readOp.leakage+darray_decoder_power.readOp.leakage+darray_wordline_power.readOp.leakage+darray_bitline_power.readOp.leakage+darray_senseamp_power.readOp.leakage+data_output_power.readOp.leakage+total_out_driver_power.readOp.leakage+darray_readControl_power.readOp.leakage+darray_writeControl_power.readOp.leakage);

		total_power_without_routing.readOp.dynamic=(*NSubbanks)*(darray_decoder_power.readOp.dynamic+darray_wordline_power.readOp.dynamic+darray_bitline_power.readOp.dynamic+darray_senseamp_power.readOp.dynamic+data_output_power.readOp.dynamic+darray_readControl_power.readOp.dynamic+darray_writeControl_power.readOp.dynamic)/FUDGEFACTOR;
		total_power_without_routing.readOp.leakage=(*NSubbanks)*(darray_decoder_power.readOp.leakage+darray_wordline_power.readOp.leakage+darray_bitline_power.readOp.leakage+darray_senseamp_power.readOp.leakage+data_output_power.readOp.leakage+darray_readControl_power.readOp.leakage+darray_writeControl_power.readOp.leakage);

		total_power_allbanks.readOp.dynamic= total_power_without_routing.readOp.dynamic + total_routing_power.readOp.dynamic/FUDGEFACTOR;
		total_power_allbanks.readOp.leakage= total_power_without_routing.readOp.leakage + total_routing_power.readOp.leakage;

		total_power.writeOp.dynamic =(subbankaddress_routing_power.writeOp.dynamic+darray_decoder_power.writeOp.dynamic+darray_wordline_power.writeOp.dynamic+darray_bitline_power.writeOp.dynamic+darray_senseamp_power.writeOp.dynamic+data_output_power.writeOp.dynamic+total_out_driver_power.writeOp.dynamic+darray_readControl_power.writeOp.dynamic+darray_writeControl_power.writeOp.dynamic)/FUDGEFACTOR;
		total_power.writeOp.leakage =(subbankaddress_routing_power.writeOp.leakage+darray_decoder_power.writeOp.leakage+darray_wordline_power.writeOp.leakage+darray_bitline_power.writeOp.leakage+darray_senseamp_power.writeOp.leakage+data_output_power.writeOp.leakage+total_out_driver_power.writeOp.leakage+darray_readControl_power.writeOp.leakage+darray_writeControl_power.writeOp.leakage);

		total_power_without_routing.writeOp.dynamic=(*NSubbanks)*(darray_decoder_power.writeOp.dynamic+darray_wordline_power.writeOp.dynamic+darray_bitline_power.writeOp.dynamic+darray_senseamp_power.writeOp.dynamic+data_output_power.writeOp.dynamic+darray_readControl_power.writeOp.dynamic+darray_writeControl_power.writeOp.dynamic)/FUDGEFACTOR;
		total_power_without_routing.writeOp.leakage=(*NSubbanks)*(darray_decoder_power.writeOp.leakage+darray_wordline_power.writeOp.leakage+darray_bitline_power.writeOp.leakage+darray_senseamp_power.writeOp.leakage+data_output_power.writeOp.leakage+darray_readControl_power.writeOp.leakage+darray_writeControl_power.writeOp.leakage);

		total_power_allbanks.writeOp.dynamic= total_power_without_routing.writeOp.dynamic + total_routing_power.writeOp.dynamic/FUDGEFACTOR;
		total_power_allbanks.writeOp.leakage= total_power_without_routing.writeOp.leakage + total_routing_power.writeOp.leakage;


		result->senseext_scale = senseext_scale;
		copy_powerDef(&result->total_power, total_power);
		copy_powerDef(&result->total_power_without_routing, total_power_without_routing);
		copy_powerDef(&result->total_power_allbanks, total_power_allbanks);
		copy_scaled_powerDef(&result->total_routing_power, total_routing_power, FUDGEFACTOR);
		copy_scaled_powerDef(&result->subbank_address_routing_power, subbankaddress_routing_power, FUDGEFACTOR);

		result->subbank_address_routing_delay = subbankaddress_routing_delay/FUDGEFACTOR;
		result->cycle_time = cycle_time/FUDGEFACTOR;
		result->access_time = access_time/FUDGEFACTOR;
		result->best_Ndwl = Ndwl;
		result->best_Ndbl = Ndbl;
		result->best_Nspd = Nspd;
		result->best_Ntwl = Ntwl;
		result->best_Ntbl = Ntbl;
		result->best_Ntspd = Ntspd;
		result->decoder_delay_data = decoder_data/FUDGEFACTOR;
		result->decoder_delay_tag = decoder_tag/FUDGEFACTOR;
		result->dec_tag_driver = decoder_tag_driver/FUDGEFACTOR;
		result->dec_tag_3to8 = decoder_tag_3to8/FUDGEFACTOR;
		result->dec_tag_inv = decoder_tag_inv/FUDGEFACTOR;
		result->dec_data_driver = decoder_data_driver/FUDGEFACTOR;
		result->dec_data_3to8 = decoder_data_3to8/FUDGEFACTOR;
		result->dec_data_inv = decoder_data_inv/FUDGEFACTOR;
		result->wordline_delay_data = wordline_data/FUDGEFACTOR;
		result->wordline_delay_tag = wordline_tag/FUDGEFACTOR;
		result->bitline_delay_data = bitline_data/FUDGEFACTOR;
		result->bitline_delay_tag = bitline_tag/FUDGEFACTOR;
		result->sense_amp_delay_tag = sense_amp_tag/FUDGEFACTOR;
		result->sense_amp_delay_data = sense_amp_data/FUDGEFACTOR;
		result->total_out_driver_delay_data = total_out_driver/FUDGEFACTOR;
		result->compare_part_delay = compare_tag/FUDGEFACTOR;
		result->drive_mux_delay = mux_driver/FUDGEFACTOR;
		result->selb_delay = selb/FUDGEFACTOR;
		result->drive_valid_delay = valid_driver/FUDGEFACTOR;
		result->data_output_delay = data_output/FUDGEFACTOR;

		copy_scaled_powerDef(&result->decoder_power_data,darray_decoder_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->wordline_power_data,darray_wordline_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->bitline_power_data,darray_bitline_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->sense_amp_power_data,darray_senseamp_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->total_out_driver_power_data,total_out_driver_power,FUDGEFACTOR);
		copy_scaled_powerDef(&result->data_output_power,data_output_power,FUDGEFACTOR);

		copy_scaled_powerDef(&result->read_control_power_data, darray_readControl_power, FUDGEFACTOR);
		copy_scaled_powerDef(&result->write_control_power_data, darray_writeControl_power, FUDGEFACTOR);

		result->darray_read_power = darray_read_power;
		result->darray_write_power = darray_write_power;
		result->tarray_read_power = tarray_read_power;
		result->tarray_write_power = tarray_write_power;

		result->precharge_delay = precharge_del/FUDGEFACTOR;
		result->data_nor_inputs = data_nor_inputs;
		result->tag_nor_inputs = tag_nor_inputs;
		area_subbanked(ADDRESS_BITS,BITOUT,parameters->num_readwrite_ports,parameters->num_read_ports,parameters->num_write_ports,Ndbl,Ndwl,Nspd,Ntbl,Ntwl,Ntspd,*NSubbanks,parameters,arearesult_subbanked,arearesult);
		arearesult->efficiency = (*NSubbanks)*(area_all_dataramcells+area_all_tagramcells)*100/(calculate_area(*arearesult_subbanked,parameters->fudgefactor)/100000000.0);
		arearesult->aspect_ratio_total = (arearesult_subbanked->height/arearesult_subbanked->width);
		arearesult->aspect_ratio_total = (arearesult->aspect_ratio_total > 1.0) ? (arearesult->aspect_ratio_total) : 1.0/(arearesult->aspect_ratio_total) ;
		arearesult->max_efficiency = max_efficiency;
		arearesult->max_aspect_ratio_total = max_aspect_ratio_total;
	}
}

void reset_device_widths()
{
	Waddrdrvn1 = 0.0;
	 Waddrdrvp1= 0.0;
	 Waddrdrvn2= 0.0;
	 Waddrdrvp2= 0.0;

	 Wdecdrivep2= 0.0;
	 Wdecdriven2= 0.0;
	 Wdecdrivep1= 0.0;
	 Wdecdriven1= 0.0;
	 Wdec3to8n = 0.0;
	 Wdec3to8p = 0.0;
	 WdecNORn  = 0.0;
	 WdecNORp  = 0.0;
	 Wdecinvn  = 0.0;
	 Wdecinvp  = 0.0;
	 WwlDrvn = 0.0;
	 WwlDrvp = 0.0;

	 Wtdecdrivep2= 0.0;
	 Wtdecdriven2= 0.0;
	 Wtdecdrivep1= 0.0;
	 Wtdecdriven1= 0.0;
	 Wtdec3to8n = 0.0;
	 Wtdec3to8p = 0.0;
	 WtdecNORn  = 0.0;
	 WtdecNORp  = 0.0;
	 Wtdecinvn  = 0.0;
	 Wtdecinvp  = 0.0;
	 WtwlDrvn = 0.0;
	 WtwlDrvp = 0.0;

	 Wbitpreequ= 0.0;

	 Wiso = 0.0;
	 Wpch= 0.0;
	 Wiso= 0.0;
	 WsenseEn= 0.0;
	 WsenseN= 0.0;
	 WsenseP= 0.0;
	 WoBufN = 0.0;
	 WoBufP = 0.0;
	 WsPch= 0.0;

	 WpchDrvp= 0.0; WpchDrvn= 0.0;
	 WisoDrvp= 0.0; WisoDrvn= 0.0;
	 WspchDrvp= 0.0; WspchDrvn= 0.0;
	 WsenseEnDrvp= 0.0; WsenseEnDrvn= 0.0;

	 WwrtMuxSelDrvn= 0.0;
	 WwrtMuxSelDrvp= 0.0;
	 WtwrtMuxSelDrvn= 0.0;
	 WtwrtMuxSelDrvp= 0.0;


	 Wtbitpreequ= 0.0;
	 Wtiso= 0.0;
	 Wtpch= 0.0;
	 Wtiso= 0.0;
	 WtsenseEn= 0.0;
	 WtsenseN= 0.0;
	 WtsenseP= 0.0;
	 WtoBufN = 0.0;
	 WtoBufP = 0.0;
	 WtsPch= 0.0;

	 WtpchDrvp= 0.0; WtpchDrvn= 0.0;
	 WtisoDrvp= 0.0; WtisoDrvn= 0.0;
	 WtspchDrvp= 0.0; WtspchDrvn= 0.0;
	 WtsenseEnDrvp= 0.0; WtsenseEnDrvn= 0.0;

	 WmuxdrvNANDn    = 0.0;
	 WmuxdrvNANDp    = 0.0;
	 WmuxdrvNORn	= 0.0;
	 WmuxdrvNORp	= 0.0;
	 Wmuxdrv3n	= 0.0;
	 Wmuxdrv3p	= 0.0;
	 Woutdrvseln	= 0.0;
	 Woutdrvselp	= 0.0;

	 Woutdrvnandn= 0.0;
	 Woutdrvnandp= 0.0;
	 Woutdrvnorn	= 0.0;
	 Woutdrvnorp	= 0.0;
	 Woutdrivern	= 0.0;
	 Woutdriverp	= 0.0;
}


