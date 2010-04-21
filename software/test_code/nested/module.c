#include <stdlib.h>
#include <stdio.h>

int b=0;

void simpleThread1(){
    int a=0;
    while(a<9000)
    {
      //fprintf(stderr, "a%d\n",a);
      a= a+1;
    }
    //fprintf(stderr, "a\n");
}

void simpleThread2(){
    int b=0;
    while(b<9000)
    {
      //fprintf(stderr, "b%d\n",b);
      b= b+1;
    }
//    fprintf(stderr, "b\n");

}

void simpleThread3(){
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

    #pragma omp parallel sections num_threads(3)
    {    
       #pragma omp section
       {
         simpleThread1();
       }
       #pragma omp section
       {
         simpleThread2();
       }
       #pragma omp section
       {
         simpleThread3();
       }
    }

    return 0;
}

