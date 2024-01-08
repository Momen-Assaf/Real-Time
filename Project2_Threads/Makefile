run: all
	./parent 

all: parent ogl shelvingTeam customer

parent: parent.c
	gcc -o parent parent.c

ogl: ogl.c
	gcc -o ogl ogl.c -lGL -lGLU -lglut -lm

shelvingTeam: shelvingTeam.c
	gcc -o shelvingTeam shelvingTeam.c

customer: customer.c
	gcc -o customer customer.c

clean:
	rm -f parent customer shelvingTeam ogl

