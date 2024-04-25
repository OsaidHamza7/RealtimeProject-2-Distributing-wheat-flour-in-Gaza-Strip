CC = gcc
CFLAGS = -g -Wall 
ARGS= arguments.txt
UILIBS = -lglut -lGLU -lGL -lm  -lrt
LIBS = -lpthread
NAMES = parent plane collecting_committee

all: parent plane collecting_committee

parent: parent.c functions.c ipcs.c 
	$(CC) $(CFLAGS) -o parent parent.c functions.c ipcs.c  $(LIBS)

plane:	plane.c functions.c ipcs.c 
	$(CC) $(CFLAGS) -o plane plane.c functions.c ipcs.c $(LIBS)

collecting_committee: collecting_committee.c functions.c ipcs.c 
	$(CC) $(CFLAGS) -o collecting_committee collecting_committee.c functions.c ipcs.c $(LIBS)

gui: gui.c
	$(CC) gui.c functions.c -o gui $(UILIBS)

run: parent
	./parent $(ARGS) 

clean:
	rm -f $(NAMES)
