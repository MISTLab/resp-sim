#include <stdlib.h>
#include <stdio.h>
#include "pthread.h"

int b;

void * simpleThread1(void *args){
    while(b<9)
    {
      while(b%2!=0);
      fprintf(stderr, "a%d\n",b);
      b= b+1;
    }
    fprintf(stderr, "a\n");
}

void * simpleThread2(void *args){
    while(b<9)
    {
      while(b%2!=1);
      fprintf(stderr, "b%d\n",b);
      b= b+1;
    }
    fprintf(stderr, "b\n");
}


int main(){
    b=0;
    fprintf(stderr,"indirizzo: %d\n", &b);
    pthread_t threadId1;
    pthread_t threadId2;
    
    pthread_create(&threadId1, NULL, simpleThread1, NULL);
    pthread_create(&threadId2, NULL, simpleThread2, NULL);

    pthread_join(threadId1,NULL);
    pthread_join(threadId2,NULL);

    return 0;
}
