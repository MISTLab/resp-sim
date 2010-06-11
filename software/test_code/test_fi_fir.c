/* FIR */

#define DIM 16
#define TAP 8

int main( void )
{
  int x0[DIM];
  int y0[DIM-TAP];
  int c0[TAP];
  int i0;
  int j0;

  int x1[DIM];
  int y1[DIM-TAP];
  int c1[TAP];
  int i1;
  int j1;

  //int code = 0;

  x0[0] = 0;
  x0[1] = 1;
  x0[2] = 2;
  x0[3] = 3;
  x0[4] = 4;
  x0[5] = 5;
  x0[6] = 6;
  x0[7] = 7;
  x0[8] = 8;
  x0[9] = 9;
  x0[10] = 10;
  x0[11] = 11;
  x0[12] = 12;
  x0[13] = 13;
  x0[14] = 14;
  x0[15] = 15;

  x1[0] = 0;
  x1[1] = 1;
  x1[2] = 2;
  x1[3] = 3;
  x1[4] = 4;
  x1[5] = 5;
  x1[6] = 6;
  x1[7] = 7;
  x1[8] = 8;
  x1[9] = 9;
  x1[10] = 10;
  x1[11] = 11;
  x1[12] = 12;
  x1[13] = 13;
  x1[14] = 14;
  x1[15] = 15;

  y0[0] = 0;
  y0[1] = 0;
  y0[2] = 0;
  y0[3] = 0;
  y0[4] = 0;
  y0[5] = 0;
  y0[6] = 0;
  y0[7] = 0;

  y1[0] = 0;
  y1[1] = 0;
  y1[2] = 0;
  y1[3] = 0;
  y1[4] = 0;
  y1[5] = 0;
  y1[6] = 0;
  y1[7] = 0;

  c0[0] = -1;
  c0[1] = 0;
  c0[2] = 1;
  c0[3] = 2;
  c0[4] = 3;
  c0[5] = 2;
  c0[6] = 1;
  c0[7] = 0;

  c1[0] = -1;
  c1[1] = 0;
  c1[2] = 1;
  c1[3] = 2;
  c1[4] = 3;
  c1[5] = 2;
  c1[6] = 1;
  c1[7] = 0;

  i0 = TAP;
  i1 = TAP;

  while( i0 < DIM )
  {
    (*(unsigned int *)(0x0D800040)) = (i0 != i1);

    y0[i0-TAP] = 0;
    y1[i1-TAP] = 0;

    (*(unsigned int *)(0x0D800040)) = (i0 != i1);


    j0 = 0;
    j1 = 0;

    while( j0 < TAP )
    {
      (*(unsigned int *)(0x0D800040)) = (j0 != j1);

      y0[i0-TAP] += c0[j0]*x0[i0-j0];
      y1[i1-TAP] += c1[j1]*x1[i1-j1];
      (*(unsigned int *)(0x0D800040)) = (y0[i0-TAP] != y1[i1-TAP]);
      (*(unsigned int *)(0x0D800040)) = (c0[j0] != c1[j1]) || (x0[i0-j0] != x1[i1-j1]) || (i0 != i1) || (j0 != j1);
      j0++;
      j1++;
      (*(unsigned int *)(0x0D800040)) = (j0 != j1);

    }
    i0++;
    i1++;
    (*(unsigned int *)(0x0D800040)) = (i0 != i1);
  }

  for(i0=0; i0< DIM-TAP; i0++) 
    (*(int *)(0x0D800044 + i0*4)) = y0[i0];
  return 0;
}
