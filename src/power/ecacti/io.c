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

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "def.h"
#include "leakage.h"
#include "areadef.h"

#define NEXTINT(a) skip(); scanf("%d",&(a));
#define NEXTFLOAT(a) skip(); scanf("%lf",&(a));

/*---------------------------------------------------------------*/

static double logtwo(double x);

extern double calculate_area(area_type,double);

double logtwo(double x)
{
if (x<=0) printf("%e\n",x);
        return( (double) (log(x)/log(2.0)) );
}

int                             /* compare result, see strcmp() */
mystricmp(const char *s1, const char *s2)       /* strings to compare, case insensitive */
{
  unsigned char u1, u2;

  for (;;)
    {
      u1 = (unsigned char)*s1++; u1 = tolower(u1);
      u2 = (unsigned char)*s2++; u2 = tolower(u2);

      if (u1 != u2)
        return u1 - u2;
      if (u1 == '\0')
        return 0;
    }
}

void processParamFile(char *fname) {

    char line[1024];
    FILE *fp;
    char *arg;
    int i;
    int explore_option = 0;

    fp = fopen(fname ,"r");

    if (fp == NULL) {
		fprintf(stderr,"#################################################\n");
    	fprintf(stderr,"WARNING: Cannot find any parameter file: %8s \n",fname);
    	fprintf(stderr,"     Assuming the following switch values: \n");
    	fprintf(stderr,"       -config			OFF (exploring all configs)\n");
    	fprintf(stderr,"       -dual_vt 		OFF \n");
    	fprintf(stderr,"       -verbose 		OFF \n");
    	fprintf(stderr,"#################################################\n");
    	/* exit(0); */
    } else {
		// parse the configuration file to detect the number of column types.
		while(!feof(fp)) {
			line[0] = '\n';

			/* read a line from the file and chop */
			fgets(line, 1024, fp);

			if (line[0] == '\0' || line[0] == '\n' || line[0] == '#')
			continue;

			if (line[strlen(line)-1] == '\n')
			line[strlen(line)-1] = '\0';

			arg = strtok(line," ");

			if (!mystricmp("-verbose", arg))
				verbose = TRUE;

			if (!mystricmp("-dualVt", arg))
				dualVt = TRUE;

			if (!mystricmp("-config", arg)) {
				explore = FALSE;

				// process line to extract the specified configuration
				// format: -config Ndwl:Nspd:Ndbl:Ntwl:Ntspd:Ntbl
				arg = strtok(NULL,":");
				i = 0;
				while ((arg != NULL) && (i<6)) {
					configArgs[i] = atoi(arg);
					i++;
					arg = strtok(NULL,":");
				}
				if (i != 6)  {
					fprintf(stderr,"ERROR: ILLEGAL SPECIFICATION OF THE CONFIG PARAMETER: %s \n",line);
					fprintf(stderr,"       Format: -config <Ndwl>:<Nspd>:<Ndbl>:<Ntwl>:<Ntspd>:<Ntbl> \n");
					exit(0);
				}
				//printf("read config arguments: %d %d %d %d %d %d \n", configArgs[0],configArgs[1],configArgs[2],configArgs[3],configArgs[4],configArgs[5]);
			}
		}
	}
	fclose(fp);
}



int input_data(int argc,char *argv[],parameter_type *parameters,double *NSubbanks)
{
   int C,B,A,ERP,EWP,RWP,NSER;
   float tech;
   double logbanks, assoc;
   double logbanksfloor, assocfloor;

	/* initialize parameters */
	explore = TRUE;
	verbose = FALSE;
	dualVt = FALSE;

   if ((argc!=7) && (argc!=6) && (argc != 9) && (argc!=10)) {
      printf("Cmd-line parameters: C B A TECH NSubbanks <paramFile>\n");
      printf("                 OR: C B A TECH RWP ERP EWP NSubbanks <ParamFile>\n");
      exit(0);
   }

   B = atoi(argv[2]);
   if ((B < 1)) {
       printf("Block size must >=1\n");
       exit(0);
   }

   if ((B*8 < BITOUT)) {
       printf("Block size must be at least %d\n", BITOUT/8);
       exit(0);
   }

   tech = atof(argv[4]);

   if (tech <= 0) {
       printf("Feature size must be > 0\n");
       exit(0);
   }
   if (argc==9 || argc==10)
   {

       RWP = atoi(argv[5]);
       ERP = atoi(argv[6]);
       EWP = atoi(argv[7]);
       NSER = ERP;

	   if (argc == 10) processParamFile(argv[9]);

       if ((RWP < 0) || (EWP < 0) || (ERP < 0)) {
	 		printf("Ports must >=0\n");
	 		exit(0);
       }
       if (RWP > 2) {
	 		printf("Maximum of 2 read/write ports\n");
	 		exit(0);
       }
       if ((RWP+ERP+EWP) < 1) {
     	  	printf("Must have at least one port\n");
       	 	exit(0);
       }

       *NSubbanks = atoi(argv[8]);

       if (*NSubbanks < 1 ) {
        	printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
        	exit(0);
       }

       logbanks = logtwo((double)(*NSubbanks));
       logbanksfloor = floor(logbanks);

       if(logbanks > logbanksfloor){
         	printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
         	exit(0);
       }
   }
   else
   {
   		RWP=1;
   	    ERP=0;
   	    EWP=0;
   	    NSER=0;

   	    *NSubbanks = atoi(argv[5]);

   	    if (*NSubbanks < 1 ) {
   	      	printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
   	      	exit(0);
   	    }
   	    logbanks = logtwo((double)(*NSubbanks));
   	    logbanksfloor = floor(logbanks);

   	    if(logbanks > logbanksfloor){
   	      	printf("Number of subbanks should be greater than or equal to 1 and should be a power of 2\n");
   	      	exit(0);
		}
	 	if (argc == 7) processParamFile(argv[6]);
   }

   C = atoi(argv[1])/((int) (*NSubbanks));
   if ((C < 64)) {
       printf("Cache size must >=64\n");
       exit(0);
   }

   if (!strcmp(argv[3],"FA"))
   {
       A=C/B;
       parameters->fully_assoc = 1;
   }
   else
     {
       	if (!strcmp(argv[3],"DM"))
        {
        	A=1;
        	parameters->fully_assoc = 0;
       	}
       	else
       	{
           	parameters->fully_assoc = 0;
           	A = atoi(argv[3]);
           	if ((A < 1)) {
            	printf("Associativity must >= 1\n");
            	exit(0);
           	}
           	assoc = logtwo((double)(A));
           	assocfloor = floor(assoc);

           	if(assoc > assocfloor){
            	printf("Associativity should be a power of 2\n");
           		exit(0);
           	}

           	if ((A > 32)) {
            	printf("Associativity must <= 32\n or try FA (fully associative)\n");
    			exit(0);
    		}
   		}
	}

	if (C/(8*B*A)<=0 && !parameters->fully_assoc) {
     	printf("Number of sets is too small:\n  Need to either increase cache size, or decrease associativity or block size\n  (or use fully associative cache)\n");
     	exit(0);
   	}

   	parameters->cache_size = C;
   	parameters->block_size = B;
   	parameters->associativity = A;
   	parameters->num_readwrite_ports = RWP;
   	parameters->num_read_ports = ERP;
   	parameters->num_write_ports = EWP;
   	parameters->num_single_ended_read_ports =NSER;
   	parameters->number_of_sets = C/(B*A);
   	parameters->fudgefactor = .8/tech;
   	parameters->tech_size=tech;

   	printf( "%d %d %d %d %d %d %d %d %lf %lf\n", C, B, A, RWP, ERP, EWP, NSER, parameters->number_of_sets, .8/tech, tech);

   	if (parameters->number_of_sets < 1) {
   	   	printf("Less than one set...\n");
   	   	exit(0);
   	}

   	return(OK);
}

void output_time_components(result_type *result,parameter_type *parameters)
{
	int A;
	double datapath,tagpath;

	datapath = result->subbank_address_routing_delay+result->decoder_delay_data+result->wordline_delay_data+result->bitline_delay_data+result->sense_amp_delay_data+result->total_out_driver_delay_data+result->data_output_delay;

	if (parameters->associativity == 1) {
		tagpath = result->subbank_address_routing_delay+result->decoder_delay_tag+result->wordline_delay_tag+result->bitline_delay_tag+result->sense_amp_delay_tag+result->compare_part_delay+result->drive_valid_delay;
	} else {
	    tagpath = result->subbank_address_routing_delay+result->decoder_delay_tag+result->wordline_delay_tag+result->bitline_delay_tag+result->sense_amp_delay_tag+result->compare_part_delay+result->drive_mux_delay+result->selb_delay+result->data_output_delay+result->total_out_driver_delay_data;
	}


	printf("\nTime Components:\n");
	printf("----------------\n");

	printf(" data side (with Output driver) (ns): %g\n",datapath/1e-9);
	if (!parameters->fully_assoc)
		printf(" tag side (with Output driver) (ns): %g\n",(tagpath)/1e-9);

	printf(" subbank address routing delay (ns): %g\n",result->subbank_address_routing_delay/1e-9);

	A=parameters->associativity;
	if (!parameters->fully_assoc) {
		printf(" decode_data (ns): %g\n",result->decoder_delay_data/1e-9);
	}
	else
	{
		printf(" tag_comparison (ns): %g\n",result->decoder_delay_data/1e-9);
	}
	printf(" wordline and bitline data (ns) : %g\n",(result->wordline_delay_data+result->bitline_delay_data)/1e-9);
	printf(" sense_amp_data (ns): %g\n",result->sense_amp_delay_data/1e-9);

	if (!parameters->fully_assoc)
	{
		printf(" decode_tag (ns): %g\n",result->decoder_delay_tag/1e-9);
		printf(" wordline and bitline tag (ns): %g\n",(result->wordline_delay_tag+result->bitline_delay_tag)/1e-9);
		printf(" sense_amp_tag (ns): %g\n",result->sense_amp_delay_tag/1e-9);
		printf(" compare time (ns): %g\n",result->compare_part_delay/1e-9);

		if (A == 1) {
			printf(" valid signal driver (ns): %g\n",result->drive_valid_delay/1e-9);
		}
		else {
			printf(" mux driver (ns): %g\n",result->drive_mux_delay/1e-9);
			printf(" sel inverter (ns): %g\n",result->selb_delay/1e-9);
		}
	}
	printf(" data output driver (ns): %g\n",result->data_output_delay/1e-9);
	printf(" total_out_driver (ns): %g\n", result->total_out_driver_delay_data/1e-9);
	printf(" total data path (without output driver) (ns): %g\n",result->subbank_address_routing_delay/1e-9+result->decoder_delay_data/1e-9+result->wordline_delay_data/1e-9+result->bitline_delay_data/1e-9+result->sense_amp_delay_data/1e-9);

	if (!parameters->fully_assoc)
	{
		if (A==1)
			printf(" total tag path is dm (ns): %g\n", result->subbank_address_routing_delay/1e-9+result->decoder_delay_tag/1e-9+result->wordline_delay_tag/1e-9+result->bitline_delay_tag/1e-9+result->sense_amp_delay_tag/1e-9+result->compare_part_delay/1e-9);
		else
			printf(" total tag path is set assoc (ns): %g\n", result->subbank_address_routing_delay/1e-9+result->decoder_delay_tag/1e-9+result->wordline_delay_tag/1e-9+result->bitline_delay_tag/1e-9+result->sense_amp_delay_tag/1e-9+result->compare_part_delay/1e-9+result->drive_mux_delay/1e-9+result->selb_delay/1e-9);
	}
}

void output_power_components(result_type *result,parameter_type *parameters,double *NSubbanks)
{
	int A;

	printf("\nPower Components (dyn, lkg mW):\n");
	printf("-------------------------------\n");

	printf("\n#NOTE: The power values are for a cache read hit \n\n");

	if (parameters->fully_assoc)
	{
		printf(" Total Power all Banks : %7.4g, %7.4g\n",result->total_power_allbanks.readOp.dynamic*1e3, result->total_power_allbanks.readOp.leakage*1e3);
		if (*NSubbanks > 2) {
			printf(" Total Power Without Routing : %7.4g, %7.4g\n",result->total_power_without_routing.readOp.dynamic*1e3, result->total_power_without_routing.readOp.leakage*1e3);
			printf(" Total Routing Power : %7.4g, %7.4g\n\n",result->total_routing_power.readOp.dynamic*1e3, result->total_routing_power.readOp.leakage*1e3);
		}
	}
	else
	{
		printf(" Total Power all Banks : %7.4g, %7.4g\n",result->total_power_allbanks.readOp.dynamic*1e3, result->total_power_allbanks.readOp.leakage*1e3);
		if (*NSubbanks > 2) {
			printf(" Total Power Without Routing : %7.4g, %7.4g\n",result->total_power_without_routing.readOp.dynamic*1e3, result->total_power_without_routing.readOp.leakage*1e3);
			printf(" Total Routing Power : %7.4g, %7.4g\n",result->total_routing_power.readOp.dynamic*1e3, result->total_routing_power.readOp.leakage*1e3);
		}
	}

	if (*NSubbanks > 2)
		printf(" Subbank Address Routing Power : %7.4g,  %7.4g\n",result->subbank_address_routing_power.readOp.dynamic*1e3, result->subbank_address_routing_power.readOp.leakage*1e3);

	A=parameters->associativity;
	if (!parameters->fully_assoc) {
		printf("\n  Data Array Split:\n");
		printf("  -----------------\n");
		printf("        decode : %7.4g, %7.4g\n",result->decoder_power_data.readOp.dynamic*1e3, result->decoder_power_data.readOp.leakage*1e3);
	}
	else
	{
		printf("  tag comparison (dyn lkg mW): %7.4g, %7.4g\n",result->decoder_power_data.readOp.dynamic*1e3, result->decoder_power_data.readOp.leakage*1e3);
	}
	printf("      wordline : %7.4g, %7.4g\n",result->wordline_power_data.readOp.dynamic*1e3, result->wordline_power_data.readOp.leakage*1e3);
	printf("       bitline : %7.4g, %7.4g\n",result->bitline_power_data.readOp.dynamic*1e3, result->bitline_power_data.readOp.leakage*1e3);
	printf("     sense_amp : %7.4g, %7.4g\n",result->sense_amp_power_data.readOp.dynamic*1e3, result->sense_amp_power_data.readOp.leakage*1e3);
	printf("  read_control : %7.4g, %7.4g\n",result->read_control_power_data.readOp.dynamic*1e3, result->read_control_power_data.readOp.leakage*1e3);
	printf(" write_control : %7.4g, %7.4g\n\n",result->write_control_power_data.readOp.dynamic*1e3, result->write_control_power_data.readOp.leakage*1e3);

	if (!parameters->fully_assoc)
	{
		printf("  Tag Array Split:\n");
		printf("  ----------------\n");
		printf("        decode : %7.4g, %7.4g\n",result->decoder_power_tag.readOp.dynamic*1e3, result->decoder_power_tag.readOp.leakage*1e3);
		printf("      wordline : %7.4g, %7.4g\n",result->wordline_power_tag.readOp.dynamic*1e3, result->wordline_power_tag.readOp.leakage*1e3);
		printf("       bitline : %7.4g, %7.4g\n",result->bitline_power_tag.readOp.dynamic*1e3, result->bitline_power_tag.readOp.leakage*1e3);
		printf("     sense_amp : %7.4g, %7.4g\n",result->sense_amp_power_tag.readOp.dynamic*1e3, result->sense_amp_power_tag.readOp.leakage*1e3);
		printf("  read_control : %7.4g, %7.4g\n",result->read_control_power_tag.readOp.dynamic*1e3, result->read_control_power_tag.readOp.leakage*1e3);
		printf(" write_control : %7.4g, %7.4g\n\n",result->write_control_power_tag.readOp.dynamic*1e3, result->write_control_power_tag.readOp.leakage*1e3);

		printf("     tag comparison : %7.4g, %7.4g\n",result->compare_part_power.readOp.dynamic*1e3, result->compare_part_power.readOp.leakage*1e3);
		if (A == 1) {
			printf("valid signal driver : %7.4g, %7.4g\n",result->drive_valid_power.readOp.dynamic*1e3, result->drive_valid_power.readOp.leakage*1e3);
		}
		else {
			printf("         mux driver : %7.4g, %7.4g\n",result->drive_mux_power.readOp.dynamic*1e3, result->drive_mux_power.readOp.leakage*1e3);
			printf("       sel inverter : %7.4g, %7.4g\n",result->selb_power.readOp.dynamic*1e3, result->selb_power.readOp.leakage*1e3);
		}
	}
	printf(" data output driver : %7.4g,  %7.4g\n",result->data_output_power.readOp.dynamic*1e3, result->data_output_power.readOp.leakage*1e3);
	if (*NSubbanks > 2)
		printf("   total_out_driver : %7.4g,  %7.4g\n", result->total_out_driver_power_data.readOp.dynamic*1e3, result->total_out_driver_power_data.readOp.dynamic*1e3);
}

void output_area_components(arearesult_type *arearesult,area_type *arearesult_subbanked,parameter_type *parameters,double *NSubbanks)
{
    printf("\nArea Components:\n");
    printf("----------------\n");
/*
    printf("Aspect Ratio Data height/width: %f\n", aspect_ratio_data);
    printf("Aspect Ratio Tag height/width: %f\n", aspect_ratio_tag);
    printf("Aspect Ratio Subbank height/width: %f\n", aspect_ratio_subbank);
    printf("Aspect Ratio Total height/width: %f\n\n", aspect_ratio_total);
*/
    printf(" Aspect Ratio Total height/width: %f\n\n", arearesult->aspect_ratio_total);

    printf(" Data array (cm^2): %f\n",calculate_area(arearesult->dataarray_area,parameters->fudgefactor)/100000000.0);
    printf(" Data predecode (cm^2): %f\n",calculate_area(arearesult->datapredecode_area,parameters->fudgefactor)/100000000.0);
    printf(" Data colmux predecode (cm^2): %f\n",calculate_area(arearesult->datacolmuxpredecode_area,parameters->fudgefactor)/100000000.0);
    printf(" Data colmux post decode (cm^2): %f\n",calculate_area(arearesult->datacolmuxpostdecode_area,parameters->fudgefactor)/100000000.0);
    printf(" Data write signal (cm^2): %f\n",(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports)*calculate_area(arearesult->datawritesig_area,parameters->fudgefactor)/100000000.0);

    printf("\n Tag array (cm^2): %f\n",calculate_area(arearesult->tagarray_area,parameters->fudgefactor)/100000000.0);
    printf(" Tag predecode (cm^2): %f\n",calculate_area(arearesult->tagpredecode_area,parameters->fudgefactor)/100000000.0);
    printf(" Tag colmux predecode (cm^2): %f\n",calculate_area(arearesult->tagcolmuxpredecode_area,parameters->fudgefactor)/100000000.0);
    printf(" Tag colmux post decode (cm^2): %f\n",calculate_area(arearesult->tagcolmuxpostdecode_area,parameters->fudgefactor)/100000000.0);
    printf(" Tag output driver decode (cm^2): %f\n",calculate_area(arearesult->tagoutdrvdecode_area,parameters->fudgefactor)/100000000.0);
    printf(" Tag output driver enable signals (cm^2): %f\n",(parameters->num_readwrite_ports+parameters->num_read_ports+parameters->num_write_ports)*calculate_area(arearesult->tagoutdrvsig_area,parameters->fudgefactor)/100000000.0);

    printf("\n Percentage of data ramcells alone of total area: %f %%\n",100*area_all_dataramcells/(arearesult->totalarea/100000000.0));
    printf(" Percentage of tag ramcells alone of total area: %f %%\n",100*area_all_tagramcells/(arearesult->totalarea/100000000.0));
    printf(" Percentage of total control/routing alone of total area: %f %%\n",100*(arearesult->totalarea/100000000.0-area_all_dataramcells-area_all_tagramcells)/(arearesult->totalarea/100000000.0));
    printf("\n Subbank Efficiency : %f\n",(area_all_dataramcells+area_all_tagramcells)*100/(arearesult->totalarea/100000000.0));
    printf(" Total Efficiency : %f\n",(*NSubbanks)*(area_all_dataramcells+area_all_tagramcells)*100/(calculate_area(*arearesult_subbanked,parameters->fudgefactor)/100000000.0));
    printf("\n Total area One Subbank (cm^2): %f\n",arearesult->totalarea/100000000.0);
    printf(" Total area subbanked (cm^2): %f\n",calculate_area(*arearesult_subbanked,parameters->fudgefactor)/100000000.0);

}

void output_init_data(parameter_type *parameters,double *NSubbanks)
{
	printf("\n--------------------------------\n");
	printf("---------- eCACTI 1.0 ----------");
	printf("\n--------------------------------\n\n");

	printf("Cache Subarray Parameters (C, B, A): (%d B, %d B, ",
				(int) (parameters->cache_size),	parameters->block_size);

	if (parameters->fully_assoc)
		printf("FA)\n");
	else
	{
		if (parameters->associativity==1)
			printf("DM)\n");
		else
			printf("%d)\n",parameters->associativity);
	}
	printf("Number of Subarrays: %d \n",(int)(*NSubbanks));
	printf("Ports (RW, R, W): (%d, %d, %d)\n",parameters->num_readwrite_ports,
					parameters->num_read_ports, parameters->num_write_ports);
	printf("Technology: %2.2f um, Vdd: %2.1f V\n\n", parameters->tech_size, Vdd_init);

	printf("#Cache configurations expressed in (Ndwl, Nspd, Ndbl, Ntwl, Ntspd, Ntbl) format \n\n");
}

void output_data(result_type *result,arearesult_type *arearesult,area_type *arearesult_subbanked,parameter_type *parameters,double *NSubbanks)
{

	if (explore)
		printf("Optimal Power-AccessTime-Area Config: ");
	else
		printf("Configuration: ");

	printf("(%d, %d, %d, %d, %d, %d)\n\n",
				result->best_Ndwl, result->best_Nspd, result->best_Ndbl, result->best_Ntwl, result->best_Ntspd, result->best_Ntbl);

	printf("Power Stats:\n");
	printf("------------\n");
	// for a read miss, there wud be a tag read, data read, line fill (tag write and data write).
	// for a write miss, there wud be a tag read and line fill (tag write and data write)
	//printf("%5.4f, %5.4f; %5.4f, %5.4f \n", result->tarray_read_power*1e3,result->darray_read_power*1e3, result->tarray_read_power*1e3,result->darray_write_power*1e3);
	printf("Read hit power: %5.4f mw\nWrite hit power: %5.4f mw\n\n",(result->tarray_read_power+result->darray_read_power)*1e3, (result->tarray_read_power+result->darray_write_power)*1e3);
	//printf("%5.4f, %5.4f, %5.4f, %5.4f; %5.4f, %5.4f, %5.4f \n\n",result->tarray_read_power*1e3,result->darray_read_power*1e3,result->tarray_write_power*1e3,result->darray_write_power*1e3,result->tarray_read_power*1e3,result->tarray_write_power*1e3,result->darray_write_power*1e3);
	printf("Read miss power: %5.4f mw\nWrite miss power: %5.4f mw\n\n",(result->tarray_read_power+result->darray_read_power+result->tarray_write_power+result->darray_write_power)*1e3,(result->tarray_read_power+result->tarray_write_power+result->darray_write_power)*1e3);

	printf("Timing Stats:\n");
	printf("-------------\n");
	printf("Access time: %g ns\n",result->access_time*1e9);
	printf("Cycle time (wave pipelined):  %g ns\n\n",result->cycle_time*1e9);

	printf("Area Stats:\n");
	printf("-----------\n");
	printf("Aspect ratio (height/width): %f\n", arearesult->aspect_ratio_total);
	printf("Total area one subarray : %f cm^2\n\n\n",arearesult->totalarea/100000000.0);

	if (verbose) {
		output_power_components(result,parameters,NSubbanks);

		output_time_components(result,parameters);

		output_area_components(arearesult,arearesult_subbanked,parameters,NSubbanks);
	}
}


