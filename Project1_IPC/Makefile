run: all
	./parent

all: parent customer shelvingTeam ogl

parent: parent.c
	gcc -o parent parent.c

customer: customer.c
	gcc -o customer customer.c

shelvingTeam: shelvingTeam.c
	gcc -o shelvingTeam shelvingTeam.c

ogl: ogl.c
	gcc -o ogl ogl.c -lGL -lGLU -lglut -lm

clean:
	rm -f parent customer shelvingTeam ogl

items: all
	./parent items
