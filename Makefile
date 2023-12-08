CC = gcc
CFLAGS = -Wall

all: parent customer cashier 
	./parent

parent: parent.c
	$(CC) $(CFLAGS) -o parent parent.c

customer: customer.c
	$(CC) $(CFLAGS) -o customer customer.c

cashier: cashier.c
	$(CC) $(CFLAGS) -o cashier cashier.c

clean:
	rm -f parent customer cashier
