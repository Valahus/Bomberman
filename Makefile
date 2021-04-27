CC=gcc
CFLAGS= -g -std=gnu99 -Wall -Werror
DEP= main.o term.o bombs.o timeutil.o linkstack.o

make: $(DEP) 
	$(CC) -o game $(DEP)  $(CFLAGS)

main.o : main.c term.h bombs.h
	$(CC) -c main.c $(CFLAGS)

term.o : term.c term.h
	$(CC) -c term.c $(CFLAGS)

bombs.o : bombs.c bombs.h term.h
	$(CC) -c bombs.c $(CFLAGS)

timeutil.o : timeutil.c timeutil.h
	$(CC) -c timeutil.c $(CFLAGS)

linkstack.o : linkstack.c linkstack.h
	$(CC) -c linkstack.c $(CFLAGS)


clean: $(DEP)
	rm -rvf $(DEP) game 
