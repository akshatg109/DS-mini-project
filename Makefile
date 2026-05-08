CC := gcc
TARGET := roulette_casino

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

BASE_CFLAGS := -Wall -Wextra -Wpedantic -std=c11 -Iinclude
RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
RAYLIB_LIBS := $(shell pkg-config --libs raylib 2>/dev/null)
RAYLIB_SYSTEM_LIBS := -lm -ldl -lpthread -lGL -lrt -lX11

ifeq ($(strip $(RAYLIB_LIBS)),)
RAYLIB_LIBS := -lraylib
endif

CFLAGS := $(BASE_CFLAGS) $(RAYLIB_CFLAGS)

.PHONY: all run debug clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(RAYLIB_LIBS) $(RAYLIB_SYSTEM_LIBS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -g -O0
debug: clean all

clean:
	rm -f $(OBJ) $(TARGET)
