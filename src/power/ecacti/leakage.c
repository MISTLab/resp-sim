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

#include<math.h>
#include<stdio.h>
#include"leakage.h"

/* Box-Mueller Method */

double box_mueller(double std_var, double value)
{
	double temp;
	double temp1;
	double random;
	random = drand48();
	temp = sqrt((double)(-2.00 * (double)log(random)));
	random = drand48();
	temp1 = cos(2.00 * M_PI * random);

	return(temp * temp1 * std_var * value);
}


/* ************************************************************************ */

/* Calculating the NMOS I Normalized Leakage From the BSIM Equation.*/
/* Also Using Box-Mueller to Find the Random Samples Due to Any Variation */
/* In any of the parameters like length, Vdd etc. */
/* ************************************************************************ */
double nmos_ileakage(double aspect_ratio, double Volt, double Vth0, double Tkelvin, double tox0)
{
	double Tox_Std_Array[No_of_Samples];
	double Vdd_Std_Array[No_of_Samples];
	double Tech_Std_Array[No_of_Samples];
	double Vthn_Std_Array[No_of_Samples];
	double Ileak_Std_Array[No_of_Samples];
	int i;
	double mean =0.0;

	if(Tox_Std || Tech_Std || Vdd_Std || Vthn_Std)
	{
		for(i =0; i<No_of_Samples;i++)
		{
		    Tox_Std_Array[i] = tox0;
		    Vdd_Std_Array[i] = Volt;
		    Tech_Std_Array[i] = tech_length0;
		    Vthn_Std_Array[i] = Vth0;
		}
	}

	if(Tox_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Tox_Std_Array[i] = tox0 + box_mueller(Tox_Std,tox0);
	  }
	if(Tech_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Tech_Std_Array[i] = tech_length0 + box_mueller(Tech_Std,tech_length0);
	  }
	if(Vdd_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Vdd_Std_Array[i] = Volt + box_mueller(Vdd_Std,Volt);
	  }
	if(Vthn_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Vthn_Std_Array[i] = Vth0 + box_mueller(Vthn_Std,Vth0);
	  }

	if(Tox_Std || Tech_Std || Vdd_Std || Vthn_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      {
	     Ileak_Std_Array[i] = nmos_ileakage_var(aspect_ratio, Vdd_Std_Array[i], Vthn_Std_Array[i],  Tkelvin,  Tox_Std_Array[i], Tech_Std_Array[i]);

	      }
	  }
	else
	  {
		return(nmos_ileakage_var(aspect_ratio,Volt,Vth0,Tkelvin, tox0, tech_length0));
	  }

	for(i =0; i<No_of_Samples;i++)
		mean += Ileak_Std_Array[i];
	mean = mean/(double)No_of_Samples;

	return mean;


}

double nmos_ileakage_var(double aspect_ratio, double Volt, double Vth0, double Tkelvin, double tox0, double tech_length)
{
	double Ileak;
	double Vthermal;
	double Vth,temp , Vnoff;
	double param1,param2,param3, param4,param5,param6;
	double temp1;
	param1 = (aspect_ratio * tech_length0 *M0n*Cox)/tech_length;
	/* Thermal Voltage */
	Vthermal =((Bk*Tkelvin)/Qparam);
	/* Vdd Fitting */
	temp =  Nb*(Volt- Volt0);
	param2 = exp(temp);
	param3 = 1-exp((-Volt/Vthermal));
	Vth =Vth0 + Vnthx * (Tkelvin-300);
	Vnoff = Vnoff0 + Nfix*(Vth0-Vthn);
	param4 = exp(((-fabs(Vth)-Vnoff)/(NEta*Vthermal)));
	temp = (tech_length0 - tech_length) * L_nmos_d ;
	param5 = exp(temp);
	temp1 = (tox0 - Tox) * Tox_nmos_e;
	param6 = exp(temp1);
	Ileak = param1*pow(Vthermal,2.0)*param2*param3*param4*param5*param6;

	return Ileak;
}

/* ************************************************************************ */

/* Calculating the PMOS I Normalized Leakage From the BSIM Equation.*/
/* Also Using Box-Mueller to Find the Random Samples Due to Any Variation */
/* In any of the parameters like length, Vdd etc. */
/* ************************************************************************ */
double pmos_ileakage(double aspect_ratio,double Volt, double Vth0,double Tkelvin,double tox0)
{

	double Tox_Std_Array[No_of_Samples];
	double Vdd_Std_Array[No_of_Samples];
	double Tech_Std_Array[No_of_Samples];
	double Vthp_Std_Array[No_of_Samples];
	double Ileak_Std_Array[No_of_Samples];
	int i;
	double mean =0.0;

	if(Tox_Std || Tech_Std || Vdd_Std || Vthp_Std) {
		for(i =0; i<No_of_Samples;i++)
	  	{
	  		Tox_Std_Array[i] = tox0;
	  		Vdd_Std_Array[i] = Volt;
	    	Tech_Std_Array[i] = tech_length0;
	    	Vthp_Std_Array[i] = Vth0;
	  	}
	}

	if(Tox_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Tox_Std_Array[i] = tox0 + box_mueller(Tox_Std,tox0);
	  }
	if(Tech_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Tech_Std_Array[i] = tech_length0 + box_mueller(Tech_Std,tech_length0);
	  }
	if(Vdd_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Vdd_Std_Array[i] = Volt + box_mueller(Vdd_Std,Volt);
	  }
	if(Vthp_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
	      	Vthp_Std_Array[i] = Vth0 + box_mueller(Vthp_Std,Vth0);
	  }

	if(Tox_Std || Tech_Std || Vdd_Std || Vthp_Std)
	  {
	    for(i =0; i<No_of_Samples;i++)
		    Ileak_Std_Array[i] =  pmos_ileakage_var(aspect_ratio, Vdd_Std_Array[i], Vthp_Std_Array[i],  Tkelvin,  Tox_Std_Array[i], Tech_Std_Array[i]);
	  }

	else
	  {
		return (pmos_ileakage_var(aspect_ratio,Volt,  Vth0, Tkelvin, tox0, tech_length0));
	  }

	for(i =0; i<No_of_Samples;i++)
		mean += Ileak_Std_Array[i];
	mean = mean/(double)No_of_Samples;

	return mean;

}

double pmos_ileakage_var(double aspect_ratio,double Volt, double Vth0,double Tkelvin,double tox0, double tech_length) {
	double Ileak;
	double Vthermal;
	double Vth, temp ,temp1,Vpoff;
	double param1,param2,param3,param4,param5,param6;
	param1 = (aspect_ratio * tech_length0 *M0p*Cox )/tech_length;
	/* Thermal Voltage */
	Vthermal =((Bk*Tkelvin)/Qparam);
	/* Vdd Fitting */
	temp =  Pb*(Volt- Volt0);
	param2 = exp(temp);
	param3 = 1-exp((-Volt/Vthermal));
	Vth =Vth0 + Vpthx * (Tkelvin-300);
	Vpoff = Vpoff0 + Pfix*(Vth0-Vthp);
	param4 = exp(((-fabs(Vth)-Vpoff)/(PEta*Vthermal)));
	temp = (tech_length0 - tech_length) * L_nmos_d ;
	param5 = exp(temp);
	temp1 = (tox0 - Tox) * Tox_nmos_e;
	param6 = exp(temp1);
	Ileak = param1*pow(Vthermal,2.0)*param2*param3*param4*param5*param6;

	return Ileak;
}

void init_tech_params(double technology) {

	double tech = technology * 1000.0;
	Tkelvin = 373.0;

	//tech = (int) ceil(technology * 1000.0);

	if (tech < 181 && tech > 179) {
		process_tech = 0.18;
		tech_length0 = 180E-9;
		M0n =   3.5E-2;      /* Zero Bias Mobility for N-Type */
		M0p =  8.0E-3 ;      /* Zero Bias Mobility for P-Type */
		Tox = 3.5E-9;

		Cox = (Eox/Tox);     /* Gate Oxide Capacitance per unit area */
		Vnoff0 = 7.5E-2 ;    /* Empirically Determined Model Parameter for N-Type */
	                         /* FIX ME */
		Vpoff0 = -2.8e-2 ;   /* Empirically Determined Model Parameter for P-Type */
  	  	Nfix =  0.22;        /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Pfix =  0.17 ;       /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */

  		Vthn =  0.3979 ;     /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) (original) */
  		Vthp =  0.4659 ;     /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) (original) */

  		Vnthx = -1.0E-3 ;    /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
  		Vpthx =  -1.0E-3 ;   /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
  		Vdd_init=  2.0 ;     /* Default Vdd. Can be Changed in leakage.c */
  		Volt0 =  1.7 ;

  		Na    =  -1.5 ;      /* Empirical param for the Vdd fit */
  		Nb    =   1.2 ;      /* Empirical param for the Vdd fit */
  		Pa    =   5.0 ;      /* Empirical param for the Vdd fit */
  		Pb    =   0.75 ;     /* Empirical param for the Vdd fit */
  		NEta =  1.5 ;        /* Sub-threshold Swing Co-efficient N-Type */
  		PEta =  1.6  ;       /* Sub-threshold Swing Co-efficient P-Type */

		/* gate Vss */
  		Vth0_gate_vss= 0.65;
  		aspect_gate_vss= 5;

		/*drowsy cache*/
  		Vdd_low= 0.6;

		/*RBB*/
  		k1_body_n= 0.5613;
  		k1_body_p= 0.5560;
  		vfi = 0.6;

  		VSB_NMOS= 0.5;
  		VSB_PMOS= 0.5;

		/* dual VT*/
  		Vt_cell_nmos_high= 0.45  ;
  		Vt_cell_pmos_high= 0.5;
  		Vt_bit_nmos_low = 0.35;
  		Vt_bit_pmos_low = 0.4;

  		L_nmos_d  = 0.1E+9;    /* Adjusting Factor for Length */
  		Tox_nmos_e  = 2.6E+9;  /* Adjusting Factor for Tox */
  		L_pmos_d  = 0.1E+9;    /* Adjusting Factor for Length */
  		Tox_pmos_e  = 2.6E+9;  /* Adjusting Factor for Tox */
	}

	/* TECH_POINT130nm */

	else if (tech < 131 && tech > 129)  {

		process_tech = 0.13;
  		tech_length0 = 130E-9;
  		M0n =  1.34E-2;      /* Zero Bias Mobility for N-Type */
  		M0p =  5.2E-3 ;      /* Zero Bias Mobility for P-Type */
  		//Tox =  3.3E-9;
  		Tox = 2.52731e-09;
  		Cox =  (Eox/Tox);    /* Gate Oxide Capacitance per unit area */
  		//Vnoff0 = -6.2E-2 ;   /* Empirically Determined Model Parameter for N-Type */
  		Vnoff0 = -1.68E-1 ;   /* Empirically Determined Model Parameter for N-Type */
  		//Vpoff0 = -0.1;       /* Empirically Determined Model Parameter for P-Type */
  		Vpoff0 = -0.28;       /* Empirically Determined Model Parameter for P-Type */
  		Nfix =   0.16 ;      /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Pfix =  0.13 ;       /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
  		Vthn =  0.3353;      /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Vthp =  0.3499 ;     /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
  		Vnthx = -0.85E-3;    /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
  		Vpthx = -0.1E-3;     /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
  		//Vdd_init =  1.5 ;    /* Default Vdd. */
  		Vdd_init =  1.08 ;    /* Default Vdd. */
  		Volt0 =  1.2;

  		Na  =    3.275 ;     /* Empirical param for the Vdd fit */
  		Nb  =     1.1 ;      /* Empirical param for the Vdd fit */
  		Pa  =     4.65 ;     /* Empirical param for the Vdd fit */
  		Pb  =     2.2 ;      /* Empirical param for the Vdd fit */
  		NEta =  1.6 ;        /* Sub-threshold Swing Co-efficient N-Type */
  		PEta = 1.8   ;       /* Sub-threshold Swing Co-efficient P-Type */


		/* gate Vss */
  		Vth0_gate_vss = 0.55;
  		aspect_gate_vss = 5;

		/*drowsy cache*/
  		Vdd_low =  0.45;

		/*RBB*/
  		k1_body_n = 0.3662;
  		k1_body_p = 0.4087;
  		vfi =  0.55;
  		VSB_NMOS = 0.45;
  		VSB_PMOS = 0.45;

		/* dual VT*/
  		Vt_cell_nmos_high = 0.38  ;
  		Vt_cell_pmos_high=  0.4;
  		Vt_bit_nmos_low = 0.28;
  		Vt_bit_pmos_low = 0.29;


  		L_nmos_d  = 0.285E+9;  /* Adjusting Factor for Length */
  		Tox_nmos_e  = 4.3E+9;  /* Adjusting Factor for Tox */
  		L_pmos_d  = 0.44E+9;   /* Adjusting Factor for Length */
  		Tox_pmos_e  = 5.0E+9;  /* Adjusting Factor for Tox */
	}

	/* TECH_POINT100nm */

     else if (tech < 101 && tech > 99)  {

  		process_tech = 0.10;
  		tech_length0 = 100E-9;
  		M0n = 1.8E-2 ;      /* Zero Bias Mobility for N-Type */
  		M0p = 5.5E-3  ;     /* Zero Bias Mobility for P-Type */
  		Tox = 2.5E-9 ;
  		Cox = (Eox/Tox);    /* Gate Oxide Capacitance per unit area */
  		Vnoff0 = -2.7E-2;   /* Empirically Determined Model Parameter for N-Type */
  		                    /* FIX ME */
  		Vpoff0 = -1.04E-1;  /* Empirically Determined Model Parameter for P-Type */
  		Nfix  = 0.18  ;     /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Pfix =  0.14   ;    /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
  		Vthn =  0.2607  ;   /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
  		Vthp =  0.3030 ;    /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
  		Vnthx = -0.77E-3 ;  /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
  		Vpthx =  -0.72E-3;  /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
  		Vdd_init = 1.2 ;    /* Default Vdd. Can be Changed for different parts in leakage.c */
  		Volt0 =  1.0;
  		Na   =   3.6 ;      /* Empirical param for the Vdd fit */
  		Nb   =   2.5  ;     /* Empirical param for the Vdd fit */
  		Pa   =    4.49 ;    /* Empirical param for the Vdd fit */
  		Pb   =    2.4 ;     /* Empirical param for the Vdd fit */

  		NEta =  1.7 ;       /* Sub-threshold Swing Co-efficient N-Type */
  		PEta =  1.7  ;      /* Sub-threshold Swing Co-efficient P-Type */


		/* gate Vss */
  		Vth0_gate_vss = 0.50;
  		aspect_gate_vss = 5;

		/*drowsy cache*/
  		Vdd_low =  0.36;

		/*RBB*/
  		k1_body_n =  0.395;
  		k1_body_p =  0.391;
  		vfi = 0.5;
  		VSB_NMOS =  0.4;
  		VSB_PMOS =  0.4 ;

		/* dual VT*/
  		Vt_cell_nmos_high =  0.31  ;
  		Vt_cell_pmos_high =  0.35;
  		Vt_bit_nmos_low  = 0.21;
  		Vt_bit_pmos_low = 0.25;


  		L_nmos_d  = 0.102E+9;   /* Adjusting Factor for Length */
  		Tox_nmos_e  = 2.5E+9;   /* Adjusting Factor for Tox */
  		L_pmos_d  = 0.21E+9;    /* Adjusting Factor for Length */
  		Tox_pmos_e  = 3.1E+9;   /* Adjusting Factor for Tox */

 	} else if (tech > 69 && tech < 71)  {

		process_tech = 0.07;
 		tech_length0 = 70E-9;
 		M0n =  3.5E-2 ;     /* Zero Bias Mobility for N-Type */
 		M0p = 8.0E-3 ;      /* Zero Bias Mobility for P-Type */
 		Tox = 1.0E-9;
 		Cox = (Eox/Tox) ;   /* Gate Oxide Capacitance per unit area */
 		Vnoff0 = -3.9E-2 ;  /* Empirically Determined Model Parameter for N-Type */
 		                    /* FIX ME */
 		Vpoff0 = -4.35E-2;  /* Empirically Determined Model Parameter for P-Type */
 		Nfix =  -0.05 ;     /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
 		Pfix =  0.07 ;      /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
 		Vthn =  0.1902  ;   /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
 		Vthp =  0.2130 ;    /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
 		Vnthx = -0.49E-3 ;  /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
 		Vpthx =  -0.54E-3;  /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
 		Vdd_init =  1.0;    /* Default Vdd. Can be Changed in leakage.c */
 		Volt0 =  0.8;
 		Na   =   -2.94 ;    /* Empirical param for the Vdd fit */
 		Nb   =    2.0  ;    /* Empirical param for the Vdd fit */
 		Pa   =    -2.245;   /* Empirical param for the Vdd fit */
 		Pb   =    2.89;     /* Empirical param for the Vdd fit */
 		NEta =  1.3;        /* Sub-threshold Swing Co-efficient N-Type */
 		PEta  = 1.4 ;       /* Sub-threshold Swing Co-efficient P-Type */

		/*gate leakage factor for 70nm*/
 		nmos_unit_leakage = 53e-12;
 		a_nmos_vdd = 7.75;
 		b_nmos_t =  0.15e-12;
 		c_nmos_tox =  11.75e9;

		pmos_unit_leakage = 22.9e-12;
 		a_pmos_vdd = 12;
 		b_pmos_t =  0.086e-12;
 		c_pmos_tox = 11.25e9;

		/* gate Vss */
 		Vth0_gate_vss =  0.45;
 		aspect_gate_vss =  5;

 	 	/*drowsy cache*/
 		Vdd_low =  0.3;

		/*RBB*/
 		k1_body_n = 0.37;
 		k1_body_p = 0.38;
 		vfi = 0.45;
 		VSB_NMOS = 0.35 ;
 		VSB_PMOS = 0.35;

		/* dual VT*/
 		Vt_cell_nmos_high =  0.29 ;
 		Vt_cell_pmos_high =  0.31;
 		Vt_bit_nmos_low = 0.19;
 		Vt_bit_pmos_low = 0.21;

 		L_nmos_d  = 0.1E+9;    /* Adjusting Factor for Length */
 		Tox_nmos_e  = 3.0E+9;  /* Adjusting Factor for Tox */
 		L_pmos_d  = 0.225E+9;  /* Adjusting Factor for Length */
 		Tox_pmos_e  = 4.0E+9;  /* Adjusting Factor for Tox */

	}

	else {
		printf("\nERROR: Technology parameters not known for %3.4f feature size \n    (Parameters available for 0.18u, 0.13, 0.10, and 0.07u technologies)\nExiting...\n\n");
		exit(0);
	}
}
