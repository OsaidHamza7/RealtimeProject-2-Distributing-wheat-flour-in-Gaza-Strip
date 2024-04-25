CC = gcc
CFLAGS = -g -Wall 
ARGS= arguments.txt
UILIBS = -lglut -lGLU -lGL -lm  -lrt
LIBS = -lpthread
NAMES = parent plane occupation 

all: parent plane occupation 

parent: parent.c functions.c ipcs.c 
	$(CC) $(CFLAGS) -o parent parent.c functions.c ipcs.c  $(LIBS)

plane:	plane.c functions.c ipcs.c 
	$(CC) $(CFLAGS) -o plane plane.c functions.c ipcs.c $(LIBS)

collecting_committee: collecting_committee.c functions.c ipcs.c 
	$(CC) $(CFLAGS) -o collecting_committee collecting_committee.c functions.c ipcs.c $(LIBS)
	
occupation:	occupation.c functions.c ipcs.c 
	$(CC) $(CFLAGS) -o occupation occupation.c functions.c ipcs.c $(LIBS)

gui: gui.c
	$(CC) gui.c functions.c -o gui $(UILIBS)

run: parent
	./parent $(ARGS) 

clean:
	rm -f $(NAMES)
