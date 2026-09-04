CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

TARGET = procview

SRC = src/main.c \
      src/memory.c \
      src/cpu.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
