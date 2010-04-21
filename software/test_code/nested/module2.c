#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int b=0;

void *simpleThread1(void *args){
    int a=0;
    while(a<9000)
    {
      //fprintf(stderr, "a%d\n",a);
      a= a+1;
    }
    //fprintf(stderr, "a\n");
}

void *simpleThread2(void *args){
    int b=0;
    while(b<9000)
    {
      //fprintf(stderr, "b%d\n",b);
      b= b+1;
    }
//    fprintf(stderr, "b\n");

}

void *simpleThread3(void *args){
    int c=0;
    while(c<9000)
    {
      //fprintf(stderr, "c%d\n",c);
      c= c+1;
    }
//    fprintf(stderr, "c\n");
}

int main(){
    b=0;
    fprintf(stderr,"indirizzo: %d\n", &b);

    pthread_t threadId1;
    pthread_t threadId2;
    pthread_t threadId3;
    
    pthread_create(&threadId1, NULL, simpleThread1, NULL);
    pthread_create(&threadId2, NULL, simpleThread2, NULL);
    pthread_create(&threadId3, NULL, simpleThread3, NULL);

    pthread_join(threadId1,NULL);
    pthread_join(threadId2,NULL);
    pthread_join(threadId3,NULL);

    return 0;
}

