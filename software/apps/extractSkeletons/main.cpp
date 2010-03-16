#include <time.h>
#include <stdio.h>
#include <math.h>
#include "contours.h"
#include "fonctions.h"
#include "variables_globales.h"

#include <iostream>
#include <sys/time.h>
#include <omp.h>

//============================================================================
//main
//============================================================================
//#define number_threads 2
int main(int argc, char* argv[])
{
	//time_t start_time, end_time;
        int number_threads = omp_get_max_threads();
        int number_working_threads = 0;
	int edgenum;
	int i,j,k,*l_init,*l,index;
        struct timeval first,  second;
        struct timezone tzp;
        int TID;
        //int shift;

        l_init = new int[number_threads];
        l = new int[number_threads];
        
        gettimeofday (&first, &tzp);

	cartesian_initialize(maxscale, distGrid, XY_mag, XY_ang);
	q = 0;

        #pragma omp parallel for default(none) \
                shared(length_edge,edgelist,distevlp,projdist,projinfo,id,length_id,XY_mag,XY_ang,distGrid,\
                        min_ip,min_jp,max_ip,max_jp) \
                private(edgenum,valdist,valproj,xychain,projtan,min_id,min_jd,max_id,max_jd,min_in,min_jn,max_in,max_jn)
        for(edgenum=0;edgenum<edgecount;edgenum++)
	{
		//printf("=================%d=================\n",edgenum);

		
		buildDistMap(edgenum, length_edge, edgelist, distGrid, maxscale,\
					&min_id, &min_jd, &max_id, &max_jd,\
					distevlp, valdist);

		normProject(edgenum, length_edge, edgelist,\
					XY_mag, XY_ang,\
					min_id, min_jd, max_id, max_jd,\
					&min_in, &min_jn, &max_in, &max_jn,\
					&projtan, xychain, valproj);

 
		Post_traitement(edgenum, length_edge, valdist, valproj, distevlp, projtan, xychain,\
						min_id, min_jd, max_id, max_jd,\
						min_in, min_jn, max_in, max_jn,\
						min_ip, min_jp, max_ip, max_jp,\
						projdist, projinfo);

		
		id_initialize(edgenum, id , length_id);
        }

        for(i=0;i<number_threads;i++)
        {
            l_init[i] = (list_length/number_threads)*i;
            l[i] = l_init[i];
        }

        #pragma omp parallel for default(none)\
                shared(projinfo,angThresh,id,length_id,min_ip,min_jp,max_ip,max_jp,projdist,\
                        evlp1,skel,parallel_data,l,number_working_threads)\
                private(edgenum,min_i,min_j,max_i,max_j,i,j,k,notnull,TID)

        for(edgenum=1;edgenum<edgecount;edgenum++)
        {
              number_working_threads = omp_get_num_threads();

              edge_compare(edgenum, projinfo, angThresh, id, length_id);

              if (length_id[edgenum] != 0)
              {
                    envelope_edgenum(edgenum,projdist,min_ip, min_jp, max_ip, max_jp,evlp1);
 
                    for(i=0;i<length_id[edgenum];i++)
                    {
                            notnull = 0;

                                //#pragma omp critical
                                
                                envelope(edgenum,i,projdist,id,\
                                                min_ip, min_jp, max_ip, max_jp,\
                                                &parallel_data[l[omp_get_thread_num()]].min_i, &parallel_data[l[omp_get_thread_num()]].min_j,\
                                                &parallel_data[l[omp_get_thread_num()]].max_i, &parallel_data[l[omp_get_thread_num()]].max_j,\
                                                evlp1,parallel_data[l[omp_get_thread_num()]].evlp2,parallel_data[l[omp_get_thread_num()]].skel,&notnull);

                                if(notnull > 0)
                                {
                                        parallel_data[l[omp_get_thread_num()]].edgenum = edgenum;
                                        l[omp_get_thread_num()]++;
                                }
                            
                    }
              }
        }

        for(i=0;i<number_working_threads;i++)
        {
            #pragma omp parallel for default(none)\
                    shared(evlp1,parallel_data,l,l_init,lutlut,lookup,distevlp,skelim,skelregion,skellist,q,i,length_skellist,edgenum_list)\
                    private(k,skelpts,skelmap,length_skelpts,length_edgepoints)
            for(k=l_init[i];k<l[i];k++)
            {

                    //printf("=================%d=================\n",parallel_data[k].edgenum);

                    bwmorph_skel(parallel_data[k].min_i,parallel_data[k].min_j,parallel_data[k].max_i,parallel_data[k].max_j,parallel_data[k].skel);
                    bwmorph_spur(parallel_data[k].min_i,parallel_data[k].min_j,parallel_data[k].max_i,parallel_data[k].max_j,parallel_data[k].skel);

                    im_edge2param(parallel_data[k].skel,1,lutlut,lookup,parallel_data[k].min_i,parallel_data[k].min_j,parallel_data[k].max_i,\
                                    parallel_data[k].max_j,skelpts,skelmap,&length_skelpts,length_edgepoints);

                    if(length_skelpts != 0)
                    {
                            #pragma omp critical
                             {
                                record_skeletons(parallel_data[k].edgenum, skelpts, &length_skelpts, length_edgepoints,distevlp,evlp1[parallel_data[k].edgenum],parallel_data[k].evlp2,\
                                                parallel_data[k].min_i,parallel_data[k].min_j,parallel_data[k].max_i,parallel_data[k].max_j,skelim,skellist,skelregion,&q,length_skellist,edgenum_list);
                             }
                    }
            }

        }

        gettimeofday (&second, &tzp);
	std::cout << "Execution time = " <<  (second.tv_sec - first.tv_sec)*1000000 + second.tv_usec - first.tv_usec << std::endl;

        for(i=0;i<q;i++)
        {
            printf("=================%d=================\n",edgenum_list[i]);
            for(j=0;j<length_skellist[i];j++)
                printf("skellist(%d,%d) = (%d,%d,%d)\n",i,j,skellist[i][j][0],skellist[i][j][1],skellist[i][j][2]);
        }

	//printf("End execution...\n");
        
    return 0;
}
