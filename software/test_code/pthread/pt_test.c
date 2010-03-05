#include <stdlib.h>
#include <stdio.h>
#include "pthread.h"

void * simpleThread(void *args){
    fprintf(stderr, "I'm a thread :-)\n");
}

int main(){
    int i = 0;
    pthread_t threadId1;
    pthread_t threadId2;

    pthread_create(&threadId1, NULL, simpleThread, NULL);
    pthread_create(&threadId2, NULL, simpleThread, NULL);

    pthread_join(threadId1, NULL);
    pthread_join(threadId2,NULL);

    return 0;
}
