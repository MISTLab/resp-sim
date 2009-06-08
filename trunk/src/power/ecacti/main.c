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

#include "def.h"
#include "areadef.h"
#include "stdio.h"
#include "leakage.h"


/*------------------------------------------------------------------------------*/

main(int argc, char *argv[])
{
    result_type result;
    arearesult_type arearesult;
    area_type arearesult_subbanked;
    parameter_type parameters;
    double NSubbanks;
    double Ileak;
    int i;

	extern int input_data (int argc, char *argv[], parameter_type *parameters, double *NSubbanks);

	extern void evaluate_config(result_type *result, arearesult_type *arearesult,
			area_type *arearesult_subbanked, parameter_type *parameters, double *NSubbanks, int *configArgs);
	extern void explore_configs(result_type *result, arearesult_type *arearesult,
			area_type *arearesult_subbanked, parameter_type *parameters, double *NSubbanks);
	extern void output_data(result_type *result, arearesult_type *arearesult,
			area_type *arearesult_subbanked, parameter_type *parameters, double *NSubbanks);



    if (input_data(argc,argv,&parameters,&NSubbanks) == ERROR) exit(0);

	printf("\nCommand line: ");
    for (i = 0; i < argc; i++)
      printf("%s ", argv[i]);
    printf("\n");

	init_tech_params(parameters.tech_size);

	output_init_data(&parameters,&NSubbanks);

	if (!explore)
		evaluate_config(&result,&arearesult,&arearesult_subbanked,&parameters,&NSubbanks, configArgs);
    else
    	explore_configs(&result,&arearesult,&arearesult_subbanked,&parameters,&NSubbanks);

    printf( "%f\n", result.total_power_allbanks.readOp.leakage);

    output_data(&result,&arearesult,&arearesult_subbanked,&parameters,&NSubbanks);
}
