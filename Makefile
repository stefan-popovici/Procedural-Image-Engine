# Runic Image & Fractal Engine

# Compilator si flag-uri
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lm

# Numele executabilului
TARGET = runic

# Detecteaza automat toate fisierele .c si genereaza lista de .o
SRCS = main.c lsystem.c ppm.c turtle.c commands.c font.c
OBJS = $(SRCS:.c=.o)
HEADERS = $(wildcard *.h)

# Regula principala
all: build

build: $(TARGET)

# Link-uirea obiectelor
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compilarea individuala a fiecarui fisier sursa
# Se recompileaza doar daca fisierul .c sau headerele s-au modificat
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Curatenie
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all build clean pack