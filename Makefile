CC = gcc
CFLAGS = -Wall -Wextra -I.

SRCDIRS = analizador_lexico analizador_sintactico gestion_de_errores sistema_de_entrada tabla_de_simbolos tabla_hash
SRCS = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
OBJS = $(SRCS:.c=.o)
DEPS = $(wildcard */*.h)

TARGET = analizadorLexicoPython

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) main.o   # Agregar main.o como dependencia
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: main.c $(DEPS)   # Regla para compilar main.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET) main.o