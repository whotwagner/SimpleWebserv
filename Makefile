CC=gcc
INC=-I./

%.o: %.c
	$(CC) $(INC) -c $<

simplewebserv: main.o http.o
	$(CC) $(INC) -o simplewebserv main.o http.o

clean:
	rm -f *.o
