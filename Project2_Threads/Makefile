run: all
	./parent

all: parent ogl

parent: parent.c
	gcc -o parent parent.c

ogl: ogl.c
	gcc -o my_program ogl.c -lGL -lGLU -lglut -lm

clean:
	rm -f parent

items: all
	./parent items
