CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -O2
LDFLAGS = -pthread

TARGET = deadlockdoctor
SRC = main.c deadlockdoctor.c scenario.c live_demo.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run:
	./$(TARGET)

safe:
	./$(TARGET) scenarios/safe.scn

deadlock:
	./$(TARGET) scenarios/deadlock.scn
