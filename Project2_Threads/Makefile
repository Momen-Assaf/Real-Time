run: all
	./parent 

all: libshm.a parent ogl shelvingTeam

parent: parent.c
	gcc -o parent parent.c -L. -lshm

ogl: ogl.c
	gcc -o ogl ogl.c -lGL -lGLU -lglut -lm -L. -lshm

shelvingTeam: shelvingTeam.c
	gcc -o shelvingTeam shelvingTeam.c -L. -lshm

libshm.a: shm.c
	gcc -c shm.c -o shm.o
	ar rcs libshm.a shm.o

clean:
	rm -f parent shelvingTeam ogl libshm.a *.o

