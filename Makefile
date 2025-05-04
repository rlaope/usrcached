CC      = gcc
CFLAGS  = -Wall -std=gnu11 -Iinclude
LDFLAGS = -lws2_32

SRC     = src/server.c src/cache.c src/http.c
OBJ     = $(SRC:.c=.o)
TARGET  = server.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q $(OBJ) $(TARGET)