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

double area_all_datasubarrays;
double area_all_tagsubarrays;
double area_all_dataramcells;
double area_all_tagramcells;
double faarea_all_subarrays ;

double aspect_ratio_data;
double aspect_ratio_tag;
double aspect_ratio_subbank;
double aspect_ratio_total;

#define Widthptondiff 4.0
#define Widthtrack    3.2
#define Widthcontact  1.6
#define Wpoly         0.8
#define ptocontact    0.4
#define stitch_ramv   6.0
#define BitHeight16x2 33.6
#define stitch_ramh   12.0
#define BitWidth16x2  192.8
#define WidthNOR1     11.6
#define WidthNOR2     13.6
#define WidthNOR3     20.8
#define WidthNOR4     28.8
#define WidthNOR5     34.4
#define WidthNOR6     41.6
#define Predec_height1    140.8
#define Predec_width1     270.4
#define Predec_height2    140.8
#define Predec_width2     539.2
#define Predec_height3    281.6
#define Predec_width3     584.0
#define Predec_height4    281.6
#define Predec_width4     628.8
#define Predec_height5    422.4
#define Predec_width5     673.6
#define Predec_height6    422.4
#define Predec_width6     718.4
#define Wwrite		  1.2
#define SenseampHeight    152.0
#define OutdriveHeight	  200.0
#define FAOutdriveHeight  229.2
#define FArowWidth	  382.8
#define CAM2x2Height_1p	  48.8
#define CAM2x2Width_1p	  44.8
#define CAM2x2Height_2p   80.8
#define CAM2x2Width_2p    76.8
#define DatainvHeight     25.6
#define Wbitdropv 	  30.0
#define decNandWidth      34.4
#define FArowNANDWidth    71.2
#define FArowNOR_INVWidth 28.0

#define FAHeightIncrPer_first_rw_or_w_port 16.0
#define FAHeightIncrPer_later_rw_or_w_port 16.0
#define FAHeightIncrPer_first_r_port       12.0
#define FAHeightIncrPer_later_r_port       12.0
#define FAWidthIncrPer_first_rw_or_w_port  16.0
#define FAWidthIncrPer_later_rw_or_w_port  9.6
#define FAWidthIncrPer_first_r_port        12.0
#define FAWidthIncrPer_later_r_port        9.6

#define tracks_precharge_p    12
#define tracks_precharge_nx2   5
#define tracks_outdrvselinv_p  3
#define tracks_outdrvfanand_p  6

typedef struct {
	double height;
	double width;
} area_type;

typedef struct {
    area_type dataarray_area,datapredecode_area;
    area_type datacolmuxpredecode_area,datacolmuxpostdecode_area;
	area_type datawritesig_area;
    area_type tagarray_area,tagpredecode_area;
    area_type tagcolmuxpredecode_area,tagcolmuxpostdecode_area;
    area_type tagoutdrvdecode_area;
    area_type tagoutdrvsig_area;
    double totalarea;
	double total_dataarea;
    double total_tagarea;
	double max_efficiency, min_efficiency, efficiency;
	double max_aspect_ratio_total, aspect_ratio_total;
} arearesult_type;

