#include "utils.h"

void printout(char text[512], int param) {
	#ifdef OUTPUT
	printf(text,param);
	#endif
}

void printresult(char text[512], char res[5]) {
	printf(text,res);
}
