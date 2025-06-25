CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -Iinclude
SRC = src/packer.c src/flags.c src/logger.c
OBJ = $(SRC:.c=.o)
OUT = SNPACK

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC)

clean:
	rm -f $(OUT) src/*.o

.PHONY: all clean