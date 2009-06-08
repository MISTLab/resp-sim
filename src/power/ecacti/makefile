LIBS = -lm
#FLAGS = -g -xs
FLAGS = -g 
CC = cc

SRCS = main.c time.c area.c io.c leakage.c

OBJS = main.o time.o area.o io.o leakage.o

all: ecacti

ecacti : main.o time.o area.o io.o leakage.o
	  ${CC} $(FLAGS) $(OBJS) -o ecacti $(LIBS)

main.o : main.c def.h areadef.h
	  ${CC} $(FLAGS) -c main.c -o main.o

time.o :  time.c def.h areadef.h leakage.h
	  ${CC} $(FLAGS) -c time.c -o time.o

area.o : area.c def.h areadef.h
	  ${CC} $(FLAGS) -c area.c -o area.o 

io.o : def.h io.c areadef.h
	  ${CC} $(FLAGS) -c io.c -o io.o

leakage.o : leakage.h leakage.c
	  ${CC} $(FLAGS) -c leakage.c -o leakage.o

clean:
	  rm *.o ecacti 

