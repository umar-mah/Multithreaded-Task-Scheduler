CC = gcc
CFLAGS = -Wall -pthread

# Default target
all: scheduler

# Link the object files and create the scheduler executable
scheduler: main.o queue.o user_tests.o
	$(CC) $(CFLAGS) -o scheduler main.o queue.o user_tests.o

# Compile main.o
main.o: main.c queue.h
	$(CC) $(CFLAGS) -c main.c

# Compile queue.o
queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c

# Compile user_tests.o
user_tests.o: user_tests.c queue.h
	$(CC) $(CFLAGS) -c user_tests.c -o user_tests.o

# Clean all generated files
clean:
	rm -f scheduler *.o

# Run the scheduler executable
run: scheduler
	./scheduler

