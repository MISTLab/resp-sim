#ifndef COEFFS_INCLUDED
#define COEFFS_INCLUDED

/*
 *      #####################################################################
 *
 *        Copyright (C) 2006  Johan H. Bodin SM6LKM
 *
 *        This software is provided 'as is', without warranty of any kind,
 *        express or implied. In no event shall the author be held liable
 *        for any damages arising from the use of this software.
 *
 *        Permission to use, copy, modify, and distribute this software and
 *        its documentation for non-commercial purposes is hereby granted,
 *        provided that the above copyright notice and this disclaimer appear
 *        in all copies and supporting documentation.
 *
 *        The software must NOT be sold or used as part of a any commercial
 *        or "non-free" product.
 *
 *      #####################################################################
 */

#define FIR_LEN_CW1K    206
#define FIR_LEN_SSB2K4  139
#define FIR_LEN_CW300H  428
#define FIR_LARGEST_LEN FIR_LEN_CW300H  // *VERY* IMPORTANT!

extern float rCw300HCoeffs_I[FIR_LEN_CW300H];
extern float rCw300HCoeffs_Q[FIR_LEN_CW300H];

extern float rCw1kCoeffs_I[FIR_LEN_CW1K];
extern float rCw1kCoeffs_Q[FIR_LEN_CW1K];

extern float rSsb2k4Coeffs_I[FIR_LEN_SSB2K4];
extern float rSsb2k4Coeffs_Q[FIR_LEN_SSB2K4];

extern float rDecIntCoeffs[64];

#endif  // COEFFS_INCLUDED
