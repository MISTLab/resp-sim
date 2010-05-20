#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

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

void *reconfThread(void *args){
	int a,b,x;
	generate(&a);
	b=*((int*)args);
	x=printValue(sum(&a,&b));
	*((int*)args) = printValue(x);
}

int main(){
    pthread_t threadId1;
    pthread_t threadId2;
    pthread_t threadId3;
    pthread_t threadId4;

    int x=5,y=10,z=15,t=20,sum=0;
//	int i;
    pthread_create(&threadId1, NULL, reconfThread, (void*)&x);
//	for (i = 0; i < 1000; i++) {}
    pthread_create(&threadId2, NULL, reconfThread, (void*)&y);
//	for (i = 0; i < 1000; i++) {}
    pthread_create(&threadId3, NULL, reconfThread, (void*)&z);
//	for (i = 0; i < 1000; i++) {}
    pthread_create(&threadId4, NULL, reconfThread, (void*)&t);
//	for (i = 0; i < 1000; i++) {}

    pthread_join(threadId1,NULL);
    sum+=x;
    pthread_join(threadId2,NULL);
    sum+=y;
    pthread_join(threadId3,NULL);
    sum+=z;
    pthread_join(threadId4,NULL);
    sum+=t;

    return sum;
}

