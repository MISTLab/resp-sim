#include <stdio.h>
#include <stdlib.h>

int printValue(int c) {
	printf("Value: %d\n",c);
	return c+1;
}

int sum(int* a, int* b) {
	printf("Summing values...\n");
	int c = (*a)+(*b);
	return c;
}

void generate(int* tmp) {
	printf("Generating value...\n");
	*tmp = 10;
}

int main() {
	int a,b,x;
	generate(&a);
	b=5;
	x=printValue(sum(&a,&b));
	return printValue(x);
}
