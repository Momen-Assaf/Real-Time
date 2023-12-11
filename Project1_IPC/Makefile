run: all
	./parent

all: parent cashier 

parent: parent.c
	gcc -o parent parent.c

cashier: cashier.c
	gcc -o cashier cashier.c

clean:
	rm -f parent cashier

items: all
	./parent items
