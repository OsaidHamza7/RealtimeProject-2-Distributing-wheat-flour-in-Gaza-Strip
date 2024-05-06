CC = gcc
CFLAGS = -g -Wall 
ARGS= arguments.txt
UILIBS = -lglut -lGLU -lGL -lm  -lrt
LIBS = -lpthread
NAMES = parent plane occupation collecting_committe splitting_worker distributing_worker family gui

all: parent plane occupation collecting_committe splitting_worker distributing_worker family gui

parent: parent.c functions.c ipcs.c 
	$(CC) $(CFLAGS) -o parent parent.c functions.c ipcs.c  $(LIBS)

plane:	plane.c functions.c ipcs.c 
	$(CC) $(CFLAGS) -o plane plane.c functions.c ipcs.c $(LIBS)

collecting_committe: collecting_committe.c functions.c ipcs.c 
	$(CC) $(CFLAGS) -o collecting_committe collecting_committe.c functions.c ipcs.c $(LIBS)

splitting_worker: splitting_worker.c functions.c ipcs.c 
	$(CC) $(CFLAGS) -o splitting_worker splitting_worker.c functions.c ipcs.c $(LIBS)	

distributing_worker: distributing_worker.c functions.c ipcs.c 
	$(CC) $(CFLAGS) -o distributing_worker distributing_worker.c functions.c ipcs.c $(LIBS)

occupation:	occupation.c functions.c ipcs.c 
	$(CC) $(CFLAGS) -o occupation occupation.c functions.c ipcs.c $(LIBS)

family: family.c functions.c ipcs.c
	$(CC) $(CFLAGS) -o family family.c functions.c ipcs.c $(LIBS)
gui: gui.c
	$(CC) gui.c functions.c ipcs.c -o gui $(UILIBS)

run: parent
	./parent $(ARGS) 

clean:
	rm -f $(NAMES)