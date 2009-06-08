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

#ifndef LEAKAGE_H
#define LEAKAGE_H

#include<stdlib.h>


/*===================================================================*/

/*
 * The following are things you probably wouldn't want to change.
 */



/* .18 technology */

/* Common for all the Technology */
#define Bk 1.38066E-23  /* Boltzman Constant */
#define Qparam 1.602E-19    /* FIXME     */
#define Eox       3.5E-11

#define No_of_Samples 10000
#define Tox_Std 0
#define Tech_Std 0
#define Vdd_Std 0
#define Vthn_Std 0
#define Vthp_Std 0

double Tkelvin;
double process_tech;
double tech_length0;
double M0n ;      /* Zero Bias Mobility for N-Type */
double M0p  ;     /* Zero Bias Mobility for P-Type */
double Tox ;
double Cox ;      /* Gate Oxide Capacitance per unit area */
double Vnoff0  ;  /* Empirically Determined Model Parameter for N-Type */
                         /* FIX ME */
double Vpoff0  ;  /* Empirically Determined Model Parameter for P-Type */
double Nfix ;     /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
double Pfix ;     /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
double Vthn  ;    /* In the equation Voff = Vnoff0 +Nfix*(Vth0-Vthn) */
double Vthp  ;    /* In the equation Voff = Vpoff0 +Pfix*(Vth0-Vthp) */
double Vnthx  ;   /* In the Equation Vth = Vth0 +Vnthx*(T-300) */
double Vpthx ;    /* In the Equation Vth = Vth0 +Vpthx*(T-300) */
double Vdd_init ; /* Default Vdd. Can be Changed in leakage.c */
double Volt0  ;
double Na  ;      /* Empirical param for the Vdd fit */
double Nb ;       /* Empirical param for the Vdd fit */
double Pa  ;      /* Empirical param for the Vdd fit */
double Pb  ;      /* Empirical param for the Vdd fit */
double NEta  ;    /* Sub-threshold Swing Co-efficient N-Type */
double PEta   ;   /* Sub-threshold Swing Co-efficient P-Type */

double L_nmos_d  ;     /* Adjusting Factor for Length */
double Tox_nmos_e  ;   /* Adjusting Factor for Tox */
double L_pmos_d ;    /* Adjusting Factor for Length */
double Tox_pmos_e ;  /* Adjusting Factor for Tox */

/* gate Vss */
double Vth0_gate_vss ;
double aspect_gate_vss;

/*drowsy cache*/
double Vdd_low ;

/*RBB*/
double k1_body_n ;
double k1_body_p ;
double vfi ;

double VSB_NMOS ;
double VSB_PMOS ;
/* dual VT*/
double Vt_cell_nmos_high ;
double Vt_cell_pmos_high ;
double Vt_bit_nmos_low ;
double Vt_bit_pmos_low ;

/* Gate lekage for 70nm */
double  nmos_unit_leakage ;
double a_nmos_vdd ;
double b_nmos_t;
double c_nmos_tox;

double pmos_unit_leakage;
double a_pmos_vdd ;
double b_pmos_t ;
double c_pmos_tox ;


/* Technology Length */

double nmos_ileakage(double aspect_ratio,double Volt,double Vth0,double Tkelvin,double tox0);

double pmos_ileakage(double aspect_ratio,double Volt,double Vth0,double Tkelvin,double tox0);

double nmos_ileakage_var(double aspect_ratio, double Volt, double Vth0, double Tkelvin, double tox0, double tech_length);

double pmos_ileakage_var(double aspect_ratio,double Volt, double Vth0,double Tkelvin,double tox0, double tech_length);

double box_mueller(double std_var, double value);

void init_tech_params(double tech);

#endif
