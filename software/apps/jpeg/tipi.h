#ifndef TIPI_H
#define TIPI_H

//#include <xsysace.h>
//#include <xsysace_l.h>
//#include <sysace_stdio.h>
#define PI 3.141592653589793

typedef struct { int intcomp1[64],intcomp2[64],intcomp3[64];}intblock;
typedef intblock *p_intblock;
typedef p_intblock *int_rows;

typedef struct {unsigned char comp1[64], comp2[64], comp3[64];} block;
typedef block *p_block;
typedef p_block *rows;


typedef struct {
		int L, H, color, maxsample,numrows,numblocks; }info;

typedef struct {
			int id, number, maxsize, code_size[17], symbols[256], codes[256];
		}huffman_tab;
		
#endif /* TIPI_H */
