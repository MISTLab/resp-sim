void RGBtoYUV(unsigned char *__restrict__ component1, unsigned char *__restrict__ component2, unsigned char *__restrict__ component3)
{
	int i;

#ifdef FLOAT_RGB
	float r,g,b;
#else
       unsigned char r,g,b;
#endif

	for (i=0;i<64;i++)
	{

#ifdef FLOAT_RGB
		r=(float)*(component1+i);
		g=(float)*(component2+i);
		b=(float)*(component3+i);
		*(component1+i)=(unsigned char)( 0.299*r+0.587*g+0.114*b+0.5);
		*(component2+i)=(unsigned char)( 0.5*b -0.16874*r -0.33126*g +128);
		*(component3+i)=(unsigned char)(0.5*r -0.41869*g -0.08131*b +128);
#else
		r=component1[i];
		g=component2[i];
		b=component3[i];
	     	component1[i]=(unsigned char)(((66*r + 129*g + 25*b + 128)>>8)+16);
		component2[i]=(unsigned char)(((-38*r - 74*g + 112*b + 128)>>8)+128);
		component3[i]=(unsigned char)(((112*r - 94*g - 18*b + 128)>>8)+128);  
#endif
	}
}

