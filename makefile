all:
	gcc -Wall -c *.c && gcc -o main.out *.o -lncurses && rm *.o