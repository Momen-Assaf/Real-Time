CC = gcc
CFLAGS = -Wall

run: all
	./parent

all: parent cashier 

parent: parent.c
	$(CC) $(CFLAGS) -o parent parent.c

cashier: cashier.c
	$(CC) $(CFLAGS) -o cashier cashier.c

clean:
	rm -f parent cashier

items: all
	./parent items
